#include <CtrlLib/CtrlLib.h>
#include <UiDesigner/Assistant/UiDesignerAssistant.h>
using namespace Upp;
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
        UiDesignerSession session; auto button=session.AddControl("UiButton"); session.Select(button);
        UiDesignerAssistantHost host(session); host.Capture("Designer");
        int history=session.Commands().GetHistoryPosition();
        AppChatTurn turn;
        ValueArray messages; messages.Add(AppChatMessage("system",host.SystemPrompt()));
        messages.Add(AppChatMessage("user","Inspect the captured selection and describe its supported properties. Then prepare, but do not apply, a configuration proposal changing its text to Hello assistant. Use inspect_nodes and describe_control before prepare_edits."));
        turn.Start(std::make_shared<AppChatDeepSeekProvider>(profile),messages,host.Tools());
        TimeStop timer; Index<String> called;
        while(turn.active && timer.Seconds()<180) {
            turn.Poll([&](const String& name,const ValueMap& args) -> Value { called.FindAdd(name); return host.Execute(name,args); });
            Sleep(10);
        }
        if(turn.active) turn.Stop();
        auto check=[&](bool ok,const char* label) { checks++; if(!ok) failed++; Cout()<<(ok?"PASS ":"FAIL ")<<label<<"\n"; };
        check(turn.error.IsEmpty(),"real provider completed bounded tool turn");
        check(called.Find("inspect_nodes")>=0 && called.Find("describe_control")>=0,"live inspection/schema tools");
        check(host.Proposals().GetCount()>0,"live validated proposal");
        check(session.Commands().GetHistoryPosition()==history,"live model cannot apply");
        // Deliberately no automatic Apply: the drawer acceptance requires a human.
        if(!turn.error.IsEmpty()) Cout()<<turn.error<<"\n";
    }
    Cout()<<"Provider="<<profile.provider<<" Model="<<profile.model<<"\n";
    Cout()<<"AssistantLiveTest checks="<<checks<<" failed="<<failed<<"\n";
    Cout()<<"Human drawer Apply/Undo acceptance remains pending.\n";
    SetExitCode(failed?1:0);
}
