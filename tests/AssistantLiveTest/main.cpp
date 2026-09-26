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
    if(FindIndex(CommandLine(), String("theme")) >= 0) {
        int checks=0,failed=0;
        auto check=[&](bool ok,const char* label){ ++checks; if(!ok) ++failed; Cout()<<(ok?"PASS ":"FAIL ")<<label<<'\n'; };
        UiDesignerSession session; UiDesignerAssistantHost host(session);
        String original=session.Theme().Serialize(false), document=Authored(session.Document());
        for(int step=0;step<2;++step) {
            host.Capture("theme");
            ValueArray messages; messages.Add(AppChatMessage("system",host.SystemPrompt()));
            messages.Add(AppChatMessage("user", step==0
                ? "Create a yellow brutalist theme for all controls in light and dark. Use an installed font, stronger bold headings than body text, square corners, strong outlines and hard offset black shadows. Propose it in Theme Studio so I can review it."
                : "Nice, keep that theme but change only the Light Accent Accordion header title to a lighter grey #626262. Keep every other field unchanged."));
            AppChatTurn turn; turn.WhenActivity=[](const String& event){Cout()<<event<<'\n';};
            turn.Start(std::make_shared<AppChatDeepSeekProvider>(profile),messages,host.Tools());
            TimeStop timer;
            while(turn.active && timer.Seconds()<180) {
                turn.Poll([&](const String& name,const ValueMap& args){
                    Value result=host.Execute(name,args);
                    Cout()<<"tool="<<name<<" ok="<<(result["ok"]==true);
                    if(result["ok"]!=true) Cout()<<" error="<<AsString(result["error"]);
                    Cout()<<'\n'; return result;
                });
                Sleep(10);
            }
            if(turn.active) turn.Stop();
            check(turn.error.IsEmpty(),"live theme turn completed within bounded execution");
            check(session.Theme().HasProposal(),"live theme candidate exists");
            check(session.Theme().Serialize(false)==original && Authored(session.Document())==document,
                  "live preview has no durable Theme or Document mutation");
            if(step == 0) {
                const auto& theme = session.Theme().GetEffective();
                check(theme.HasStyleOverride("Light|control|UiButton|Standard","font_face") &&
                      theme.GetStyleOverride("Light|panel|UiGroupPanel|Standard","title_font_bold") == true &&
                      theme.GetStyleOverride("Light|control|UiButton|Standard","shadow_mode") == "Hard" &&
                      theme.GetStyleOverride("Light|control|UiButton|Standard","shadow_enabled") == true,
                      "live design includes typography and hard shadows, beyond palette changes");
            }
            if(!turn.error.IsEmpty()) Cout()<<turn.error<<'\n';
            if(!session.Theme().HasProposal()) break;
        }
        check(session.Theme().GetEffective().GetStyleOverride("Light|control|UiAccordion|Accent","header_title_color")==Color(98,98,98),
              "live refinement reaches exact requested recipe field");
        Value result=host.ApplyPending();
        check(result["ok"]==true && session.Theme().CanUndo(),"trusted test approval keeps proposed Theme");
        check(session.Theme().Undo() && session.Theme().Serialize(false)==original,"one Theme Undo restores initial snapshot");
        Cout()<<"Provider="<<profile.provider<<" Model="<<profile.model<<'\n';
        Cout()<<"AssistantThemeLiveTest checks="<<checks<<" failed="<<failed<<'\n';
        SetExitCode(failed?1:0); return;
    }
    int checks=0,failed=0;
    bool app_shell=FindIndex(CommandLine(),String("app-shell"))>=0;
    for(int scenario=app_shell?3:0;scenario<(app_shell?4:3);scenario++) {
        UiDesignerSession session;
        String original=Authored(session.Document());
        UiDesignerAssistantHost host(session); host.Capture("Designer");
        int history=session.Commands().GetHistoryPosition();
        AppChatTurn turn;
        turn.WhenActivity = [](const String& event) { Cout()<<event<<'\n'; };
        ValueArray messages; messages.Add(AppChatMessage("system",host.SystemPrompt()));
        const char* prompt=scenario==3
            ? "Create a simple app interface similar to a codec style of application."
            : scenario==0
            ? "Create a simple dialog box template with just an OK and cancel perhaps a with a heading that I can use as a template."
            : scenario==1 ? "Create a simple dialog box template with just an OK and cancel perhaps with a heading using a title card and an OK and cancel at the bottom."
            : "Create a dialog template with a Label as the heading at the top, an expanding empty body panel, and OK and Cancel at the bottom. Use only a Label for the heading, not a TitleCard.";
        Cout()<<"Scenario "<<scenario+1<<": "<<prompt<<'\n';
        messages.Add(AppChatMessage("user",prompt));
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
        if(scenario==3) {
            bool layout=false;
            for(const auto& node:session.Document().GetNodes())
                layout |= node.type=="UiGridLayout" || node.type=="UiBoxLayout";
            check(layout && session.Document().GetNodes().GetCount()>4,"app interface has a composed layout and visible controls");
        }
        else check(heading && ok && cancel,"exact prompt yields heading plus OK/Cancel visual template");
        if(scenario==1) {
            bool title=false,grid=false,panel=false,actions=false;
            for(const auto& node:session.Document().GetNodes()) {
                title|=node.type=="UiTitleCard";
                grid|=node.type=="UiGridLayout" && node.GetProperty("rows",0)==3;
                panel|=node.type=="UiPanel" && node.GetProperty("height_mode","")=="Expand";
                actions|=node.type=="UiBoxLayout" && node.GetProperty("direction","")=="H" && node.GetProperty("grid_row",-1)==2;
            }
            check(title && grid && panel && actions,"requested TitleCard uses three-row layout and expanding body above bottom actions");
        }
        if(scenario==2) {
            bool title=false,top_label=false;UiDesignerNodeId label_id=0;
            for(const auto& node:session.Document().GetNodes()) {
                title|=node.type=="UiTitleCard";
                if(node.type=="UiLabel") {
                    const auto* parent=session.Document().Find(node.parent);
                    top_label=parent && (parent->type=="UiGridLayout" ? node.GetProperty("grid_row",-1)==0 : !parent->children.IsEmpty() && parent->children[0]==node.id);
                    label_id=node.id;
                }
            }
            check(!title && top_label,"explicit Label is the top heading, with no TitleCard substitution");
            int count=session.Document().GetNodes().GetCount();String prior=Authored(session.Document());
            String proposal_id=host.Proposals().Top().id;host.Capture("Designer");ValueArray refinement;
            refinement.Add(AppChatMessage("system",host.SystemPrompt()));
            refinement.Add(AppChatMessage("system","Trusted UI refinement reference: "+AsJSON(host.RefinementContext(proposal_id))));
            refinement.Add(AppChatMessage("user","Change the existing heading label text to Account settings. Keep the same layout and controls; do not insert another dialog."));
            turn.Start(std::make_shared<AppChatDeepSeekProvider>(profile),refinement,host.Tools());timer.Reset();
            while(turn.active && timer.Seconds()<180){turn.Poll([&](const String& n,const ValueMap& a){return host.Execute(n,a);});Sleep(10);}
            if(turn.active)turn.Stop();
            check(turn.error.IsEmpty() && Authored(session.Document())==prior,"live refinement prepares without mutation");
            Value result=host.ApplyPending();const auto* label=session.Document().Find(label_id);
            check(result["ok"]==true && label && label->GetProperty("text","")=="Account settings" && session.Document().GetNodes().GetCount()==count,"live refinement edits existing heading without inserting duplicate controls");
            check(session.Undo() && Authored(session.Document())==prior,"refinement Undo preserves the original dialog");
        }
        check(applied && session.Undo() && Authored(session.Document())==original,"one-step Undo restores blank authored design");
        if(!turn.error.IsEmpty()) Cout()<<turn.error<<"\n";
    }
    Cout()<<"Provider="<<profile.provider<<" Model="<<profile.model<<"\n";
    Cout()<<"AssistantLiveTest checks="<<checks<<" failed="<<failed<<"\n";
    Cout()<<"Human drawer Apply/Undo acceptance remains pending.\n";
    SetExitCode(failed?1:0);
}
