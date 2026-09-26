#include <AppChat/AppChat.h>
using namespace Upp;
static int checks = 0, failed = 0;
static void Check(bool ok, const char* name) { checks++; if(!ok) { failed++; Cout() << "FAIL " << name << "\n"; } }
static String Chunk(const String& delta, const String& reason = "null") {
    return "data: {\"choices\":[{\"delta\":" + delta + ",\"finish_reason\":" + reason + "}]}\n\n";
}
class CorrelatedProvider : public AppChatScriptedProvider {
public:
    bool correlated = false;
    AppChatReply Complete(const ValueArray& messages, const ValueArray& tools, const AppChatLimits& limits,
        std::atomic<bool>& cancel, const Function<void(String)>& visible) override {
        if(position == 1) correlated = messages.GetCount() == 4 && messages[2]["role"] == "tool" && messages[2]["tool_call_id"] == "call-1" && messages[3]["role"] == "system";
        return AppChatScriptedProvider::Complete(messages,tools,limits,cancel,visible);
    }
};
class CancelProvider : public AppChatProvider {
public:
    std::atomic<bool> saw_cancel{false};
    AppChatReply Complete(const ValueArray&,const ValueArray&,const AppChatLimits&,
        std::atomic<bool>& cancel,const Function<void(String)>& visible) override {
        for(int i=0;i<200 && !cancel;i++) Sleep(1);
        saw_cancel=cancel.load(); AppChatReply r;
        r.message=AppChatMessage("assistant","late result"); visible("late result"); return r;
    }
};
static ValueMap Batch(std::initializer_list<const char*> names) {
    ValueArray calls; int i=0;
    for(const char* name:names) { ValueMap c,f; c.Set("id",AsString(++i)); c.Set("type","function");
        f.Set("name",name); f.Set("arguments","{}"); c.Set("function",f); calls.Add(c); }
    ValueMap m=AppChatMessage("assistant",""); m.Set("tool_calls",calls); return m;
}
CONSOLE_APP_MAIN {
    {
        auto provider=std::make_shared<AppChatScriptedProvider>();
        for(int i=0;i<5;i++) provider->replies.Add(Batch({"read"}));
        auto final=Batch({"prepare_composition"}); final.Set("content","One valid proposal is ready.");
        provider->replies.Add(final);
        AppChatTurn turn; turn.limits.rounds=6; String activity;
        turn.WhenActivity=[&](const String& line){activity<<line<<'\n';};
        turn.Start(provider,ValueArray(),ValueArray()); TimeStop timer;
        while(turn.active && timer.Seconds()<3) {
            turn.Poll([](const String& name,const ValueMap&)->Value {
                ValueMap result; result.Set("ok",name!="prepare_composition");
                if(name=="prepare_composition") result.Set("error","Unknown control type: UiColumn");
                return result;
            }); Sleep(1);
        }
        Check(turn.GetRound()==6 && turn.GetCallsUsed()==6 && !turn.error.IsEmpty(),"rejected final-round preparation stays bounded");
        Check(turn.CompletionNotice(0).Find("No changes were prepared")>=0 &&
              turn.CompletionNotice(0).Find("Unknown control type")>=0,"completion contradicts unsupported model readiness with actual rejection");
        Check(activity.Find("prepare_composition ERROR: Unknown control type")>=0,"activity includes actionable host rejection");
        Check(turn.CompletionNotice(1).Find("1 proposal(s) ready")>=0,"later limit failure still exposes an existing valid proposal");
    }
    {
        auto original=std::make_shared<AppChatScriptedProvider>();
        original->replies.Add(Batch({"inspect_context","retrieve_skill","search_controls","search_controls","list_presets"}));
        original->replies.Add(Batch({"inspect_hierarchy","describe_control"}));
        original->replies.Add(Batch({"search_controls","search_controls","search_controls","invalid_UiColumn"}));
        original->replies.Add(Batch({"describe_control","describe_control","search_controls","search_controls"}));
        original->replies.Add(Batch({"describe_controls","prepare_composition"}));
        AppChatTurn trace; trace.limits.calls=16; int invoked=0; String activity;
        trace.WhenActivity=[&](const String& line){activity<<line<<'\n';};
        trace.Start(original,ValueArray(),ValueArray()); TimeStop watch;
        while(trace.active && watch.Seconds()<3) { trace.Poll([&](const String& name,const ValueMap&)->Value {
            invoked++; ValueMap result; result.Set("ok",name!="invalid_UiColumn"); return result; }); Sleep(1); }
        Check(invoked==15 && trace.GetCallsUsed()==15 && trace.GetRound()==5,"replayed live discovery sequence stops before overflowing batch");
        Check(trace.error.Find("15 tool calls used; 2 more requested; limit 16 (1 remaining)")>=0,"limit error reports used requested limit and remaining");
        Check(activity.Find("invalid_UiColumn ERROR")>=0 && activity.Find("requested=2")>=0,"trace distinguishes rejected discovery and blocked batch");
        auto exact=std::make_shared<AppChatScriptedProvider>();
        for(int i=0;i<4;i++) exact->replies.Add(Batch({"read","read","read","read"}));
        exact->replies.Add(AppChatMessage("assistant","Complete"));
        trace.Start(exact,ValueArray(),ValueArray()); watch.Reset(); invoked=0;
        while(trace.active && watch.Seconds()<3) { trace.Poll([&](const String&,const ValueMap&)->Value {invoked++; return ValueMap();}); Sleep(1); }
        Check(invoked==16 && trace.error.IsEmpty(),"exactly sixteen calls are permitted");
        trace.limits=AppChatLimits();
        auto boundary=std::make_shared<AppChatScriptedProvider>();
        for(int i=0;i<4;++i) boundary->replies.Add(Batch({"read","read","read","read","read","read"}));
        boundary->replies.Add(Batch({"read"}));
        trace.Start(boundary,ValueArray(),ValueArray()); watch.Reset(); invoked=0;
        while(trace.active && watch.Seconds()<3) { trace.Poll([&](const String&,const ValueMap&)->Value {++invoked;return ValueMap();});Sleep(1); }
        Check(trace.limits.rounds==8 && invoked==24 && trace.error.Find("24 tool calls used; 1 more requested; limit 24")>=0,
              "default recovery allowance permits twenty-four calls and rejects the next batch");
    }
    AppChatStream stream;
    String data = Chunk("{\"content\":\"Hello\",\"reasoning_content\":\"private\"}") + Chunk("{}", "\"stop\"") + "data: [DONE]\n\n";
    for(int i = 0; i < data.GetCount(); i++) stream.Feed(data.Mid(i, 1), 4096);
    Check(stream.Text() == "Hello", "streaming projection excludes reasoning");
    auto reply = stream.Finish(); Check(reply.error.IsEmpty(), "fragmented SSE completes");
    Check(reply.message["reasoning_content"] == "private", "protocol reasoning retained only internally");
    AppChatStream partial; partial.Feed(Chunk("{\"content\":\"partial\"}"), 4096);
    Check(!partial.Finish().error.IsEmpty(), "disconnect rejects incomplete message");
    AppChatStream malformed; Check(!malformed.Feed("data: {broken}\n", 4096), "malformed response");
    AppChatStream bounded; Check(!bounded.Feed(data, 3), "bounded output");
    AppChatStream limited; limited.Feed(Chunk("{}", "\"length\"") + "data: [DONE]\n", 4096);
    Check(!limited.Finish().error.IsEmpty(), "token exhaustion explicit");
    AppChatProfile profile; String error;
    profile.endpoint = "https://api.deepseek.com/chat/completions"; profile.model = "test"; profile.credential_env = "UID_ASSISTANT_TEST_MISSING_CREDENTIAL";
    Check(!profile.Validate(error) && error.Find("missing") >= 0, "missing credential before request");
    profile.endpoint = "https://unrelated.example/chat/completions";
    Check(!profile.Validate(error), "credentials never sent to arbitrary host");
    auto scripted = std::make_shared<CorrelatedProvider>();
    ValueMap call; call.Set("id", "call-1"); call.Set("type", "function");
    ValueMap f; f.Set("name", "inspect"); f.Set("arguments", "{\"value\":7}"); call.Set("function", f);
    ValueArray calls; calls.Add(call); ValueMap m = AppChatMessage("assistant", ""); m.Set("tool_calls", calls);
    scripted->replies.Add(m); scripted->replies.Add(AppChatMessage("assistant", "Done"));
    AppChatTurn turn; ValueArray messages; messages.Add(AppChatMessage("user", "hello"));
    Check(turn.Start(scripted, messages, ValueArray()), "start scripted turn");
    int invoked = 0; TimeStop timer;
    while(turn.active && timer.Seconds() < 3) {
        turn.Poll([&](const String& name, const ValueMap& a) -> Value { invoked++; Check(name == "inspect" && a["value"] == 7, "alternative host receives complete args"); return "ok"; }); Sleep(1);
    }
    if(turn.active || !turn.error.IsEmpty()) Cout() << "Turn diagnostic: active=" << turn.active << " error=" << turn.error << " invoked=" << invoked << "\n";
    Check(!turn.active && turn.error.IsEmpty() && turn.text == "Done" && invoked == 1, "bounded continuation completes");
    Check(scripted->correlated,"tool results preserve provider correlation");
    auto bad = std::make_shared<AppChatScriptedProvider>(); f.Set("arguments", "{\"value\":"); call.Set("function", f); calls.Set(0, call); m.Set("tool_calls", calls); bad->replies.Add(m);
    turn.Start(bad, messages, ValueArray()); timer.Reset();
    while(turn.active && timer.Seconds() < 3) { turn.Poll([&](const String&, const ValueMap&) -> Value { invoked++; return "bad"; }); Sleep(1); }
    Check(invoked == 1 && !turn.error.IsEmpty(), "partial arguments never execute");
    auto rounds = std::make_shared<AppChatScriptedProvider>(); f.Set("arguments", "{}"); call.Set("function", f); calls.Set(0,call); m.Set("tool_calls",calls); rounds->replies.Add(m);
    turn.limits.rounds = 1; turn.Start(rounds,messages,ValueArray()); timer.Reset();
    while(turn.active && timer.Seconds() < 3) { turn.Poll([](const String&,const ValueMap&) -> Value { return "ok"; }); Sleep(1); }
    Check(turn.error.Find("limit") >= 0, "round exhaustion");
    turn.Stop(); Check(!turn.active, "cancel terminal state");
    auto cancellable=std::make_shared<CancelProvider>();
    {
        AppChatTurn cancelled; cancelled.Start(cancellable,messages,ValueArray()); cancelled.Stop();
        cancelled.Poll([&](const String&,const ValueMap&) -> Value { failed++; return Value(); });
        Check(!cancelled.active && cancelled.text.IsEmpty(),"cancel suppresses late projection and dispatch");
    }
    Check(cancellable->saw_cancel,"destruction joins cancelled worker without a host pointer");
    AppChatStream tool_stream;
    tool_stream.Feed(Chunk("{\"tool_calls\":[{\"index\":0,\"id\":\"c1\",\"function\":{\"name\":\"inspect\",\"arguments\":\"{\"}}]}"),8192);
    tool_stream.Feed(Chunk("{\"tool_calls\":[{\"index\":0,\"function\":{\"arguments\":\"} \"}}]}","\"tool_calls\"")+"data: [DONE]\n",8192);
    auto assembled=tool_stream.Finish();
    Check(assembled.error.IsEmpty() && assembled.message["tool_calls"][0]["function"]["arguments"] == "{} ","streamed arguments assembled before execution");
    Cout() << "AppChatTests checks=" << checks << " failed=" << failed << "\n"; SetExitCode(failed ? 1 : 0);
}
