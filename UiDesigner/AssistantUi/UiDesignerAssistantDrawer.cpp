#include "UiDesignerAssistantDrawer.h"
namespace Upp {
UiDesignerAssistantDrawer::UiDesignerAssistantDrawer(UiDesignerSession& s) : host(s) {
    Ctrl* children[] = { &context, &profile_label, &provider, &model, &credential, &transcript,
        &proposal_detail, &composer, &send, &stop, &collapse, &profile_toggle, &configure, &proposals, &apply, &dismiss, &affected };
    for(Ctrl* c : children) Add(*c);
    transcript.SetReadOnly(); proposal_detail.SetReadOnly();
    turn.WhenActivity = [=](const String& activity) {
        context.SetText(activity); history << "Activity: " << activity << "\n";
    };
    provider.Add("DeepSeek", "DeepSeek"); provider.Add("OpenRouter", "OpenRouter"); provider.SetData("OpenRouter");
    model.Tip("Provider model ID (must support tool calling)");
    model.SetPlaceholder("Model ID (tool-capable)");
    composer.SetPlaceholder("Discuss a design or request a proposal. Enter sends; Shift+Enter adds a line.");
    credential.SetTextUtf8("OPENROUTER_API_KEY"); credential.Tip("Environment-variable name only; never paste a key");
    send.SetText("Send"); stop.SetText("Stop"); collapse.SetText("Collapse");
    profile_toggle.SetText("Profile..."); configure.SetText("Save profile");
    apply.SetText("Apply"); dismiss.SetText("Dismiss"); affected.SetText("Select affected");
    context.SetText("Ready for a design question or proposal request.");
    send.WhenAction = [=] { Submit(); }; composer.WhenSend = [=] { Submit(); };
    stop.WhenAction = [=] { Stop(); }; collapse.WhenAction = [=] { WhenCollapse(); };
    configure.WhenAction = [=] { Configure(); };
    profile_toggle.WhenAction = [=] { profile_open = !profile_open; SyncProfileSummary(); Layout(); Refresh(); };
    proposals.WhenSelectData = [=](const Value&) { UpdateProposal(); };
    provider.WhenSelectData = [=](const Value& value) {
        credential.SetTextUtf8(value == "OpenRouter" ? "OPENROUTER_API_KEY" : "DEEPSEEK_API_KEY");
        configured = false; SyncProfileSummary();
    };
    model.WhenChange = [=] { configured = false; SyncProfileSummary(); };
    credential.WhenChange = [=] { configured = false; SyncProfileSummary(); };
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
    SyncProfileSummary();
    SetTimeCallback(-100, [=] { Tick(); }, 1);
    RefreshTheme();
}
UiDesignerAssistantDrawer::~UiDesignerAssistantDrawer() { KillTimeCallback(1); Stop(); }
void UiDesignerAssistantDrawer::SyncProfileSummary() {
    if(configured)
        profile_label.SetText(profile.provider + " / " + profile.model);
    else if(profile_open)
        profile_label.SetText("Assistant profile settings");
    else
        profile_label.SetText("Assistant profile not configured");
    profile_toggle.SetText(profile_open ? "Close" : "Profile...");
}
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
    if(configured) profile_open = false;
    SyncProfileSummary(); Layout();
    context.SetText(configured ? "Profile saved. Requests share captured design context with " + profile.provider
                               : "Unable to save application profile");
}
void UiDesignerAssistantDrawer::Submit() {
    if(turn.active) return;
    String input = TrimBoth(composer.GetTextUtf8()), error;
    if(input.IsEmpty()) return;
    if(!configured || !profile.Validate(error)) { context.SetText(error.IsEmpty() ? "Select Use profile before sending." : error); return; }
    if(input.GetCount() > 16384) { context.SetText("Message too long (16 KiB limit)."); return; }
    if(!host.SameDocument()) { conversation.Clear(); host.CancelPending(); }
    String workspace = Workspace ? Workspace() : String("Designer");
    ValueMap scope = host.Capture(workspace);
    submitted = "Context captured for " + workspace + ".";
    context.SetText(submitted);
    context.Tip("Captured request context (diagnostic): " + AsJSON(scope));
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
        if(!turn.error.IsEmpty()) {
            history << turn.error << "\n"; context.SetText(turn.error); context.Tip(turn.error);
            for(const auto& proposal : host.Proposals()) if(proposal.status=="pending") {
                history << "A prepared proposal is still available for review. Nothing was applied automatically.\n"; break;
            }
        }
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
    proposal_detail.Clear(); proposal_detail.Tip("");
    for(const auto& p : host.Proposals()) if(p.id == proposals.GetData()) {
        String detail = p.summary + "\n\nStatus: " + p.status;
        if(!p.receipt.IsEmpty()) detail << "\n\n" << p.receipt;
        proposal_detail.SetTextUtf8(detail);
        proposal_detail.Tip("Exact proposal scope (diagnostic): " + AsJSON(p.args, true));
        break;
    }
    Layout();
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
    UiButton* buttons[] = { &stop, &configure, &dismiss };
    for(auto* b : buttons) b->SetCustomStyle(UiTheme::ResolveButton(UiRole::Standard));
    send.SetCustomStyle(UiTheme::ResolveButton(UiRole::Accent));
    apply.SetCustomStyle(UiTheme::ResolveButton(UiRole::Accent));
    profile_toggle.SetCustomStyle(UiTheme::ResolveButton(UiRole::Subtle));
    collapse.SetCustomStyle(UiTheme::ResolveButton(UiRole::Subtle));
    affected.SetCustomStyle(UiTheme::ResolveButton(UiRole::Subtle));
    Refresh();
}
void UiDesignerAssistantDrawer::LeftDown(Point p, dword) { if(p.y < 8) { drag_y = GetMousePos().y; initial_height = GetSize().cy; SetCapture(); } }
void UiDesignerAssistantDrawer::MouseMove(Point, dword) { if(HasCapture()) WhenHeight(initial_height + drag_y - GetMousePos().y); }
void UiDesignerAssistantDrawer::LeftUp(Point, dword) { if(HasCapture()) ReleaseCapture(); }
void UiDesignerAssistantDrawer::Layout() {
    int w = GetSize().cx, h = GetSize().cy, gap = 6, row = 28;
    int right = w - gap;
    collapse.SetRect(max(gap, right - 94), 10, 94, row); right -= 100;
    stop.SetRect(max(gap, right - 70), 10, 70, row); right -= 76;
    profile_toggle.SetRect(max(gap, right - 90), 10, 90, row); right -= 96;
    profile_label.Show(); profile_label.SetRect(gap + 4, 10, max(0, right - gap - 4), row);

    int body_y;
    if(profile_open) {
        provider.Show(); model.Show(); credential.Show(); configure.Show();
        provider.SetRect(gap, 44, 120, row);
        model.SetRect(132, 44, 250, row);
        credential.SetRect(388, 44, 210, row);
        configure.SetRect(604, 44, 108, row);
        context.SetRect(gap, 76, max(0, w - 2 * gap), 24);
        body_y = 104;
    }
    else {
        provider.Hide(); model.Hide(); credential.Hide(); configure.Hide();
        context.SetRect(gap, 42, max(0, w - 2 * gap), 24);
        body_y = 70;
    }

    int composer_h = 62;
    int composer_y = max(body_y + 40, h - composer_h - gap);
    int body = max(40, composer_y - body_y - gap);
    bool have_proposal = proposal_count > 0;
    int proposal_w = have_proposal ? min(380, max(300, w / 3)) : 0;
    int transcript_w = max(0, w - 2 * gap - (have_proposal ? proposal_w + gap : 0));
    transcript.SetRect(gap, body_y, transcript_w, body);

    proposals.Show(have_proposal); proposal_detail.Show(have_proposal);
    apply.Show(have_proposal); dismiss.Show(have_proposal); affected.Show(have_proposal);
    if(have_proposal) {
        int px = gap + transcript_w + gap;
        proposals.SetRect(px, body_y, proposal_w, row);
        apply.SetRect(px, body_y + 34, 66, row);
        dismiss.SetRect(px + 72, body_y + 34, 76, row);
        affected.SetRect(px + 154, body_y + 34, 112, row);
        proposal_detail.SetRect(px, body_y + 68, proposal_w, max(0, body - 68));
    }

    int send_w = 80, send_h = 36;
    composer.SetRect(gap, composer_y, max(0, w - 3 * gap - send_w), composer_h);
    send.SetRect(max(gap, w - gap - send_w), composer_y + (composer_h - send_h) / 2, send_w, send_h);
}
}
