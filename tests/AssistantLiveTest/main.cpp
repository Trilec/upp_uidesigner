#include <CtrlLib/CtrlLib.h>
#include <UiDesigner/Assistant/UiDesignerAssistant.h>
using namespace Upp;
static String Authored(const UiDesignerDocument& d) { ValueMap value=UiDesignerDocumentToValue(d); value.RemoveKey("revision"); return AsJSON(value); }
GUI_APP_MAIN {
    AppChatProfile profile;
    Value saved=ParseJSON(LoadFile(ConfigFile("uidesigner-assistant.json")));
    if(!saved.Is<ValueMap>()) { Cout()<<"NOT RUN: configure the Assistant profile in Designer first\n"; SetExitCode(2); return; }
    profile.provider=AsString(saved["provider"]); profile.endpoint=AsString(saved["endpoint"]);
    profile.model=AsString(saved["model"]); profile.credential_env=AsString(saved["credential_env"]);
    String error;
    if(!profile.Validate(error)) { Cout()<<"NOT RUN: "<<error<<"\n"; SetExitCode(2); return; }
    int checks=0,failed=0;
    {
        UiDesignerSession session;
        String original=Authored(session.Document());
        UiDesignerAssistantHost host(session); host.Capture("Designer");
        int history=session.Commands().GetHistoryPosition();
        AppChatTurn turn;
        turn.WhenActivity = [](const String& event) { Cout()<<event<<'\n'; };
        ValueArray messages; messages.Add(AppChatMessage("system",host.SystemPrompt()));
        messages.Add(AppChatMessage("user","Create a simple dialog box template with just an OK and cancel perhaps a with a heading that I can use as a template."));
        turn.Start(std::make_shared<AppChatDeepSeekProvider>(profile),messages,host.Tools());
        TimeStop timer; Index<String> called;
        while(turn.active && timer.Seconds()<180) {
            turn.Poll([&](const String& name,const ValueMap& args) -> Value {
                called.FindAdd(name); Value result=host.Execute(name,args);
                // Diagnostics contain tool/schema identifiers and validation errors only.
                if(name=="describe_control") Cout()<<"Schema: "<<AsString(args["type"])<<'\n';
                if(result["ok"]==false) Cout()<<"Validation: "<<AsString(result["error"])<<'\n';
                return result;
            });
            Sleep(10);
        }
        if(turn.active) turn.Stop();
        auto check=[&](bool ok,const char* label) { checks++; if(!ok) failed++; Cout()<<(ok?"PASS ":"FAIL ")<<label<<"\n"; };
        check(turn.error.IsEmpty(),"real provider completed bounded tool turn");
        check(called.Find("list_presets")>=0 || called.Find("describe_control")>=0 || called.Find("describe_controls")>=0,"live relevant capability discovery");
        check(host.Proposals().GetCount()>0,"live validated proposal");
        check(session.Commands().GetHistoryPosition()==history && Authored(session.Document())==original,"live model cannot apply");
        // Trusted test-driver approval, separate from the model's allowlisted tools.
        // The visible drawer Apply/Undo check is performed separately.
        bool applied=false;
        if(!host.Proposals().IsEmpty()) {
            Value result=host.Apply(host.Proposals().Top().id); applied=result["ok"]==true;
        }
        check(applied && session.Commands().GetHistoryPosition()==history+1,"trusted Apply commits once");
        bool heading=false,ok=false,cancel=false;
        for(const auto& node:session.Document().GetNodes()) {
            String text=ToLower(AsString(node.GetProperty("text","")));
            heading|=(node.type=="UiLabel" && !text.IsEmpty()) || (node.type=="UiTitleCard" && !AsString(node.GetProperty("title","")).IsEmpty());
            ok|=node.type=="UiButton" && text=="ok"; cancel|=node.type=="UiButton" && text=="cancel";
        }
        check(heading && ok && cancel,"exact prompt yields heading plus OK/Cancel visual template");
        check(applied && session.Undo() && Authored(session.Document())==original,"one-step Undo restores blank authored design");
        if(!turn.error.IsEmpty()) Cout()<<turn.error<<"\n";
    }
    Cout()<<"Provider="<<profile.provider<<" Model="<<profile.model<<"\n";
    Cout()<<"AssistantLiveTest checks="<<checks<<" failed="<<failed<<"\n";
    Cout()<<"Human drawer Apply/Undo acceptance remains pending.\n";
    SetExitCode(failed?1:0);
}
