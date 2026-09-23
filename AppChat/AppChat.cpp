#include "AppChat.h"
namespace Upp {
ValueMap AppChatMessage(const String& role, const String& content) {
    ValueMap m; m.Set("role", role); m.Set("content", content); return m;
}
bool AppChatProfile::Validate(String& error) const {
    if(!((provider == "DeepSeek" && endpoint == "https://api.deepseek.com/chat/completions") ||
         (provider == "OpenRouter" && endpoint == "https://openrouter.ai/api/v1/chat/completions")))
        error = "Configure the documented HTTPS endpoint for DeepSeek or OpenRouter.";
    else if(model.IsEmpty() || credential_env.IsEmpty())
        error = "Configure a model and credential environment-variable reference.";
    else if(GetEnv(credential_env).IsEmpty())
        error = "The configured credential environment variable is missing. No request sent.";
    else { error.Clear(); return true; }
    return false;
}
AppChatReply AppChatScriptedProvider::Complete(const ValueArray&, const ValueArray&,
    const AppChatLimits&, std::atomic<bool>& cancel, const Function<void(String)>& visible) {
    AppChatReply r;
    if(cancel) r.error = "Cancelled";
    else if(position >= replies.GetCount()) r.error = "Script exhausted";
    else { r.message = replies[position++]; visible(AsString(r.message["content"])); }
    return r;
}
bool AppChatStream::Feed(const String& chunk, int limit) {
    bytes += chunk.GetCount();
    if(bytes > limit) { error = "Provider output limit exhausted"; return false; }
    pending.Cat(chunk);
    int p;
    while((p = pending.Find('\n')) >= 0) {
        String line = TrimBoth(pending.Left(p)); pending = pending.Mid(p + 1);
        if(!line.StartsWith("data:")) continue;
        String data = TrimBoth(line.Mid(5));
        if(data == "[DONE]") { done = true; continue; }
        Value v = ParseJSON(data);
        if(!v.Is<ValueMap>()) { error = "Malformed streaming response"; return false; }
        Value choices = v["choices"];
        if(!choices.Is<ValueArray>()) { error = "Missing response choices"; return false; }
        ValueArray a = choices;
        if(a.IsEmpty()) continue;
        Value d = a[0]["delta"];
        if(!IsNull(a[0]["finish_reason"])) finish = AsString(a[0]["finish_reason"]);
        if(!IsNull(d["content"])) content.Cat(AsString(d["content"]));
        if(!IsNull(d["reasoning_content"])) reasoning.Cat(AsString(d["reasoning_content"]));
        // OpenRouter can require opaque reasoning detail blocks on continuation,
        // including when the selected upstream ignores disabled reasoning.
        if(d["reasoning_details"].Is<ValueArray>()) for(const Value& item : (ValueArray)d["reasoning_details"]) {
            if(!item.Is<ValueMap>() || !IsNumber(item["index"])) { error = "Malformed reasoning detail"; return false; }
            int index = item["index"];
            if(index < 0 || index >= 128) { error = "Reasoning detail limit exhausted"; return false; }
            while(reasoning_details.GetCount() <= index) reasoning_details.Add(ValueMap());
            ValueMap detail = reasoning_details[index], delta = item;
            for(int j = 0; j < delta.GetCount(); j++) {
                String key = AsString(delta.GetKey(j)); Value value = delta.GetValue(j);
                if((key == "text" || key == "data" || key == "signature") && detail[key].Is<String>() && value.Is<String>())
                    value = AsString(detail[key]) + AsString(value);
                detail.Set(key, value);
            }
            reasoning_details.Set(index, detail);
        }
        if(d["tool_calls"].Is<ValueArray>()) for(const Value& item : (ValueArray)d["tool_calls"]) {
            if(!IsNumber(item["index"])) { error = "Missing tool index"; return false; }
            int index = item["index"];
            if(index < 0 || index >= 16) { error = "Tool call limit exhausted"; return false; }
            while(calls.GetCount() <= index) calls.Add(ValueMap());
            ValueMap c = calls[index];
            if(!IsNull(item["id"])) c.Set("id", item["id"]);
            c.Set("type", "function");
            ValueMap f;
            if(c["function"].Is<ValueMap>()) f = c["function"];
            for(const char* key : {"name", "arguments"})
                if(!IsNull(item["function"][key]))
                    f.Set(key, (IsNull(f[key]) ? String() : AsString(f[key])) + AsString(item["function"][key]));
            c.Set("function", f); calls.Set(index, c);
        }
    }
    return true;
}
AppChatReply AppChatStream::Finish() {
    AppChatReply r;
    if(!error.IsEmpty()) r.error = error;
    else if(!done || (finish != "stop" && finish != "tool_calls"))
        r.error = "Incomplete response or output token limit exhausted";
    else {
        r.message = AppChatMessage("assistant", content);
        if(!calls.IsEmpty()) r.message.Set("tool_calls", calls);
        if(!reasoning.IsEmpty()) r.message.Set("reasoning_content", reasoning);
        if(!reasoning_details.IsEmpty()) r.message.Set("reasoning_details", reasoning_details);
    }
    return r;
}
AppChatTurn::~AppChatTurn() { Stop(); if(worker.joinable()) worker.join(); }
void AppChatTurn::Stop() {
    if(mailbox) mailbox->cancel = true;
    if(active) error = "Cancelled; unapplied proposals remain unapplied.";
    active = false;
}
bool AppChatTurn::Start(std::shared_ptr<AppChatProvider> p, const ValueArray& m, const ValueArray& t) {
    if(active) return false;
    if(worker.joinable()) {
        if(mailbox && !mailbox->done) { error = "Cancellation is still completing."; return false; }
        worker.join();
    }
    provider = p; messages = m; descriptors = t; round = calls = 0;
    text.Clear(); error.Clear(); active = true; Launch(); return active;
}
void AppChatTurn::Launch() {
    if(++round > limits.rounds || AsJSON(messages).GetCount() > limits.request_bytes) {
        error = "Conversation request/round limit exhausted"; active = false; return;
    }
    mailbox = std::make_shared<Mailbox>();
    auto box = mailbox; auto p = provider;
    ValueArray m = messages, t = descriptors; AppChatLimits l = limits;
    worker = std::thread([box, p, m, t, l]() {
        try {
            box->reply = p->Complete(m, t, l, box->cancel, [box](String s) {
                std::lock_guard<std::mutex> lock(box->mutex); box->visible = s;
            });
        } catch(...) { box->reply.error = "Provider failed"; }
        box->done = true;
    });
}
void AppChatTurn::Poll(const Function<Value(const String&, const ValueMap&)>& execute) {
    if(!active || !mailbox) return;
    { std::lock_guard<std::mutex> lock(mailbox->mutex); text = mailbox->visible; }
    if(!mailbox->done) return;
    worker.join();
    AppChatReply& r = mailbox->reply;
    if(!r.error.IsEmpty()) { error = r.error; active = false; return; }
    ValueArray tc;
    if(r.message["tool_calls"].Is<ValueArray>()) tc = r.message["tool_calls"];
    if(tc.IsEmpty()) { text = AsString(r.message["content"]); active = false; return; }
    if(calls + tc.GetCount() > limits.calls) { error = "Tool limit exhausted"; active = false; return; }
    Index<String> ids;
    // Validate the entire call envelope before executing any host tool.
    for(const Value& c : tc) {
        String id = AsString(c["id"]);
        Value args = ParseJSON(AsString(c["function"]["arguments"]));
        if(!c["id"].Is<String>() || id.IsEmpty() || ids.Find(id) >= 0 || !args.Is<ValueMap>() ||
           !c["function"]["name"].Is<String>()) {
            error = "Malformed/partial tool arguments or duplicate correlation ID"; active = false; return;
        }
        ids.Add(id);
    }
    messages.Add(r.message);
    for(const Value& c : tc) {
        if(mailbox->cancel) { Stop(); return; }
        Value value;
        try { value = execute(AsString(c["function"]["name"]), (ValueMap)ParseJSON(AsString(c["function"]["arguments"]))); }
        catch(...) { error = "Host tool failed; no automatic retry"; active = false; return; }
        ValueMap result = AppChatMessage("tool", AsJSON(value));
        result.Set("tool_call_id", c["id"]); messages.Add(result); calls++;
    }
    Launch();
}
}
