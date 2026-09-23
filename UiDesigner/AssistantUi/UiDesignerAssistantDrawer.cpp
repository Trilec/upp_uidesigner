#include "UiDesignerAssistantDrawer.h"
namespace Upp {
UiDesignerAssistantDrawer::UiDesignerAssistantDrawer(UiDesignerSession& s) : host(s) {
    Ctrl* children[] = { &context, &profile_label, &provider, &model, &credential, &transcript,
        &proposal_detail, &composer, &send, &stop, &collapse, &configure, &proposals, &apply, &dismiss, &affected };
    for(Ctrl* c : children) Add(*c);
    transcript.SetReadOnly(); proposal_detail.SetReadOnly();
    provider.Add("DeepSeek", "DeepSeek"); provider.Add("OpenRouter", "OpenRouter"); provider.SetData("OpenRouter");
    model.Tip("Provider model ID (must support tool calling)");
    model.SetPlaceholder("Model ID (tool-capable)");
    composer.SetPlaceholder("Discuss a design or request a proposal. Enter sends; Shift+Enter adds a line.");
    credential.SetTextUtf8("OPENROUTER_API_KEY"); credential.Tip("Environment-variable name only; never paste a key");
    send.SetText("Send"); stop.SetText("Stop"); collapse.SetText("Collapse"); configure.SetText("Use profile");
    apply.SetText("Apply"); dismiss.SetText("Dismiss"); affected.SetText("Show affected");
    profile_label.SetText("Provider / model / key variable");
    context.SetText("Configure a provider before sending. Enter sends; Shift+Enter adds a line.");
    send.WhenAction = [=] { Submit(); }; composer.WhenSend = [=] { Submit(); };
    stop.WhenAction = [=] { Stop(); }; collapse.WhenAction = [=] { WhenCollapse(); };
    configure.WhenAction = [=] { Configure(); };
    proposals.WhenSelectData = [=](const Value&) { UpdateProposal(); };
    provider.WhenSelectData = [=](const Value& value) {
        credential.SetTextUtf8(value == "OpenRouter" ? "OPENROUTER_API_KEY" : "DEEPSEEK_API_KEY");
        configured = false;
    };
    model.WhenChange = [=] { configured = false; };
    credential.WhenChange = [=] { configured = false; };
    apply.WhenAction = [=] {
        if(turn.active) return;
        Value result = host.Apply(AsString(proposals.GetData()));
        history << "\nApply: " << AsJSON(result) << "\n"; UpdateProposal();
    };
    dismiss.WhenAction = [=] { host.Dismiss(AsString(proposals.GetData())); UpdateProposal(); };
    affected.WhenAction = [=] { host.ShowAffected(AsString(proposals.GetData())); };
    Value saved = ParseJSON(LoadFile(ConfigFile("uidesigner-assistant.json")));
    if(saved.Is<ValueMap>()) {
        profile.provider = AsString(saved["provider"]); profile.endpoint = AsString(saved["endpoint"]);
        profile.model = AsString(saved["model"]); profile.credential_env = AsString(saved["credential_env"]);
        provider.SetData(profile.provider); model.SetTextUtf8(profile.model); credential.SetTextUtf8(profile.credential_env);
        configured = true;
    }
    SetTimeCallback(-100, [=] { Tick(); }, 1);
    RefreshTheme();
}
UiDesignerAssistantDrawer::~UiDesignerAssistantDrawer() { KillTimeCallback(1); Stop(); }
void UiDesignerAssistantDrawer::Configure() {
    if(turn.active) return;
    String ref = TrimBoth(credential.GetTextUtf8());
    if(ref.IsEmpty() || ref.GetCount() > 80) { context.SetText("Enter an environment-variable name."); return; }
    for(char c : ref) if(!IsAlNum(c) && c != '_') { context.SetText("Use a variable name, not a key."); return; }
    profile.provider = AsString(provider.GetData()); profile.model = TrimBoth(model.GetTextUtf8()); profile.credential_env = ref;
    profile.endpoint = profile.provider == "OpenRouter" ? "https://openrouter.ai/api/v1/chat/completions" : "https://api.deepseek.com/chat/completions";
    if(profile.model.IsEmpty()) { context.SetText("Enter the provider model ID."); return; }
    ValueMap settings; settings.Set("provider", profile.provider); settings.Set("endpoint", profile.endpoint);
    settings.Set("model", profile.model); settings.Set("credential_env", profile.credential_env);
    configured = SaveFile(ConfigFile("uidesigner-assistant.json"), AsJSON(settings, true));
    context.SetText(configured ? "Profile selected. Send shares captured design context with " + profile.provider : "Unable to save application profile");
}
void UiDesignerAssistantDrawer::Submit() {
    if(turn.active) return;
    String input = TrimBoth(composer.GetTextUtf8()), error;
    if(input.IsEmpty()) return;
    if(!configured || !profile.Validate(error)) { context.SetText(error.IsEmpty() ? "Select Use profile before sending." : error); return; }
    if(input.GetCount() > 16384) { context.SetText("Message too long (16 KiB limit)."); return; }
    if(!host.SameDocument()) { conversation.Clear(); host.CancelPending(); }
    ValueMap scope = host.Capture(Workspace ? Workspace() : String("Designer"));
    submitted = "Scope: " + AsJSON(scope);
    context.SetText(submitted); context.Tip(submitted);
    ValueArray request; request.Add(AppChatMessage("system", host.SystemPrompt()));
    for(const Value& m : conversation) request.Add(m);
    request.Add(AppChatMessage("user", input));
    if(turn.Start(std::make_shared<AppChatDeepSeekProvider>(profile), request, host.Tools())) {
        conversation.Add(AppChatMessage("user", input));
        history << "\nYou: " << input << "\n";
        composer.SetTextUtf8(""); composer.SetFocus(); was_active = true;
    } else context.SetText(turn.error);
}
void UiDesignerAssistantDrawer::Stop() { turn.Stop(); host.CancelPending(); UpdateProposal(); }
void UiDesignerAssistantDrawer::Tick() {
    if(was_active && !host.SameDocument()) { Stop(); conversation.Clear(); }
    turn.Poll([=](const String& name, const ValueMap& args) { return host.Execute(name, args); });
    if(was_active && !turn.active) {
        history << "Assistant: " << turn.text << "\n";
        if(!turn.error.IsEmpty()) history << turn.error << "\n";
        else conversation.Add(AppChatMessage("assistant", turn.text));
        while(conversation.GetCount() > 20) conversation.Remove(0);
        was_active = false;
    }
    String projection = history + (turn.active ? "Assistant: " + turn.text : String());
    if(projection != last_projection) { transcript.SetTextUtf8(projection); last_projection = projection; }
    if(proposal_count != host.Proposals().GetCount()) {
        for(int i = proposal_count; i < host.Proposals().GetCount(); i++) {
            const auto& p = host.Proposals()[i]; proposals.Add(p.summary, p.id); proposals.SetDataSilently(p.id);
        }
        proposal_count = host.Proposals().GetCount(); UpdateProposal();
    }
    send.Enable(!turn.active); configure.Enable(!turn.active); stop.Enable(turn.active);
    provider.Enable(!turn.active); model.Enable(!turn.active); credential.Enable(!turn.active);
    apply.Enable(!turn.active && proposal_count > 0);
}
void UiDesignerAssistantDrawer::UpdateProposal() {
    for(const auto& p : host.Proposals()) if(p.id == proposals.GetData()) {
        proposal_detail.SetTextUtf8(p.summary + "\nStatus: " + p.status + "\n" + p.receipt +
            "\nExact scope: " + AsJSON(p.args, true));
    }
}
void UiDesignerAssistantDrawer::Paint(Draw& w) {
    UiPanel::Paint(w); w.DrawRect(0, 0, GetSize().cx, 4, SColorShadow());
}
void UiDesignerAssistantDrawer::RefreshTheme() {
    SetCustomStyle(UiTheme::ResolvePanel(UiPanelRole::Surface));
    UiBaseEdit::Style edit = UiTheme::ResolveEdit(UiTheme::GetContext(), UiRole::Standard);
    edit.show_readonly_bg = false;
    transcript.SetCustomStyle(edit); proposal_detail.SetCustomStyle(edit); composer.SetCustomStyle(edit);
    model.SetCustomStyle(edit); credential.SetCustomStyle(edit);
    UiButton* buttons[] = { &send, &stop, &collapse, &configure, &apply, &dismiss, &affected };
    for(auto* b : buttons) b->SetCustomStyle(UiTheme::ResolveButton(UiRole::Standard));
    Refresh();
}
void UiDesignerAssistantDrawer::LeftDown(Point p, dword) { if(p.y < 8) { drag_y = GetMousePos().y; initial_height = GetSize().cy; SetCapture(); } }
void UiDesignerAssistantDrawer::MouseMove(Point, dword) { if(HasCapture()) WhenHeight(initial_height + drag_y - GetMousePos().y); }
void UiDesignerAssistantDrawer::LeftUp(Point, dword) { if(HasCapture()) ReleaseCapture(); }
void UiDesignerAssistantDrawer::Layout() {
    int w = GetSize().cx, h = GetSize().cy, gap = 6, row = 28;
    provider.SetRect(6, 10, 120, row); model.SetRect(132, 10, 220, row); credential.SetRect(358, 10, 200, row);
    configure.SetRect(564, 10, 100, row); stop.SetRect(max(670,w-186), 10, 80, row); collapse.SetRect(max(756,w-100), 10, 94, row);
    profile_label.Hide(); context.SetRect(gap, 42, max(0,w-12), 24);
    int split = w * 3 / 5, body = max(40,h-154);
    transcript.SetRect(gap, 70, max(0,split-12), body);
    proposals.SetRect(split, 70, max(0,w-split-6), row);
    proposal_detail.SetRect(split, 104, max(0,w-split-6), max(0,body-68));
    apply.SetRect(split, 70+body-28, 70, 28); dismiss.SetRect(split+76,70+body-28,80,28); affected.SetRect(split+162,70+body-28,120,28);
    composer.SetRect(gap, h-76, max(0,w-106),70); send.SetRect(max(0,w-94),h-76,88,70);
}
}
