#include <UiDesigner/AssistantUi/UiDesignerAssistantDrawer.h>
using namespace Upp;
static int checks = 0, failed = 0;
static void Check(bool ok, const char* name) { checks++; if(!ok) { failed++; Cout() << "FAIL " << name << "\n"; } }
static bool Ok(const Value& r) { return r["ok"] == true; }
static String Authored(const UiDesignerDocument& document) { ValueMap v=UiDesignerDocumentToValue(document); v.RemoveKey("revision"); return AsJSON(v); }
static String ProposalId(const Value& r) { return AsString(r["result"]["id"]); }
static ValueMap Edit(UiDesignerNodeId node, const String& property, const Value& value, const String& kind = "configuration") {
    ValueMap e; e.Set("node",node); e.Set("property",property); e.Set("value",value); e.Set("kind",kind); return e;
}
static ValueMap Group(const ValueArray& edits) { ValueMap a; a.Set("summary","Test proposal"); a.Set("edits",edits); return a; }
GUI_APP_MAIN {
  {
    AppChatConversationView view;view.SetRect(0,0,500,200);
    String full="A literal & label with a long paragraph. ";for(int i=0;i<6;i++)full<<"More words remain available when folded. ";
    auto& card=view.AddMessage("Assistant",full,"stable-id");
    int collapsed=card.MeasureAndArrange(300);card.SetExpanded(true);int expanded=card.MeasureAndArrange(300);
    Check(expanded>collapsed && card.GetText()==full,"reusable card folds measured text without losing full content");
    card.AddAction("review","Review",[]{});view.JumpTo("stable-id");
    Check(view.GetCount()==1,"history navigation retains one authoritative message");
    view.ClearMessages();Check(view.GetCount()==0,"conversation view clears child controls");
  }
  {
    UiDesignerSession session;UiDesignerAssistantHost host(session);host.Capture("Designer");
    ValueMap skill_args;skill_args.Set("id","layout-v2");Value skill=host.Execute("retrieve_skill",skill_args);
    Value proposal=host.Execute("prepare_composition",skill["result"]["prepare_composition_example"]);
    String id=ProposalId(proposal);Check(host.ProposalState(id)=="Ready","fresh proposal presents Ready");
    Check(Ok(host.Apply(id)),"refinement fixture applies");
    Value scope=host.RefinementContext(id);
    Check(scope["existing_nodes"].Is<ValueArray>() && ((ValueArray)scope["existing_nodes"]).GetCount()==5 && IsNull(scope["draft"]),"applied refinement references actual nodes, not insertion draft");
    String authored=Authored(session.Document());int history=session.Commands().GetHistoryPosition();
    host.ClearConversation();
    Check(host.Proposals().IsEmpty() && Authored(session.Document())==authored && session.Commands().GetHistoryPosition()==history,"clear history retains authored design and Undo ownership");
    Check(!Ok(host.Apply(id)) && session.Undo(),"cleared proposal cannot replay and normal Undo remains available");
    host.Capture("Designer");
    proposal=host.Execute("prepare_composition",skill["result"]["prepare_composition_example"]);
    id=ProposalId(proposal);host.SupersedePending(id);
    Check(host.ProposalState(id)=="revised" && !Ok(host.Apply(id)),"superseded pending draft cannot apply");
    proposal=host.Execute("prepare_composition",skill["result"]["prepare_composition_example"]);
    id=ProposalId(proposal);session.NewDocument();
    Check(host.ProposalState(id)=="Needs review" && !Ok(host.Apply(id)),"document switch exposes stale proposal without enabling Apply");
  }
  {
    UiDesignerSession dialog; UiDesignerAssistantHost host(dialog); host.Capture("Designer");
    String initial=Authored(dialog.Document()); ValueMap args; args.Set("id","layout-v2");
    Value skill=host.Execute("retrieve_skill",args);
    Check(!Ok(host.ApplyPending()),"text Apply explains missing proposal");
    ValueMap invalid=ParseJSON(AsJSON(skill["result"]["titlecard_dialog_example"]));
    ValueArray invalid_items=invalid["items"]; ValueMap invalid_heading=invalid_items[1];
    invalid_heading.Set("grid_row",3); invalid_items.Set(1,invalid_heading); invalid.Set("items",invalid_items);
    Check(!Ok(host.Execute("prepare_composition",invalid)) && Authored(dialog.Document())==initial,
          "out-of-range grid placement is rejected without mutation");
    Value proposal=host.Execute("prepare_composition",skill["result"]["titlecard_dialog_example"]);
    if(!Ok(proposal)) Cout()<<"TitleCard example validation: "<<AsJSON(proposal)<<'\n';
    Check(Ok(proposal) && Authored(dialog.Document())==initial,"TitleCard three-row example validates without mutation");
    Check(Ok(host.ApplyPending()),"explicit human text Apply uses canonical proposal commit");
    bool heading=false,body=false,actions=false,spacer=false;
    for(const auto& node:dialog.Document().GetNodes()) {
        heading|=node.type=="UiTitleCard" && node.GetProperty("grid_row",-1)==0;
        body|=node.type=="UiPanel" && node.GetProperty("grid_row",-1)==1 && node.GetProperty("height_mode","")=="Expand";
        actions|=node.type=="UiBoxLayout" && node.GetProperty("grid_row",-1)==2 && node.GetProperty("direction","")=="H";
        spacer|=node.type=="Spacer" && node.GetProperty("h_sizing","")=="Fill";
    }
    Check(heading && body && actions && spacer,"dialog uses requested layout and expanding body/right-aligned actions");
    Check(!Ok(host.ApplyPending()) && dialog.Undo() && Authored(dialog.Document())==initial,"text Apply cannot repeat and one Undo restores blank");
    host.Capture("Designer");
    host.Execute("prepare_composition",skill["result"]["titlecard_dialog_example"]);
    host.Execute("prepare_composition",skill["result"]["titlecard_dialog_example"]);
    Check(!Ok(host.ApplyPending()) && Authored(dialog.Document())==initial,"ambiguous text Apply requires explicit proposal selection");
  }
  {
    UiDesignerSession dialog; UiDesignerAssistantHost host(dialog); host.Capture("Designer");
    String initial=Authored(dialog.Document()); ValueMap args; args.Set("id","layout-v2");
    Value skill=host.Execute("retrieve_skill",args);
    Check(Ok(skill),"versioned dialog guidance available");
    ValueMap example=skill["result"]["prepare_composition_example"];
    ValueMap invalid=ParseJSON(AsJSON(example)); ValueArray invalid_items=invalid["items"];
    ValueMap invalid_item=invalid_items[0], invalid_props=invalid_item["properties"];
    invalid_props.Set("name","dialog"); invalid_item.Set("properties",invalid_props);
    invalid_items.Set(0,invalid_item); invalid.Set("items",invalid_items);
    Value rejected=host.Execute("prepare_composition",invalid);
    Check(!Ok(rejected) && AsString(rejected["error"]).Find("UiBoxLayout.name")>=0 && host.Proposals().IsEmpty(),"identity rejection identifies field without mutation or proposal");
    ValueArray types; types.Add("UiBoxLayout"); types.Add("UiLabel"); types.Add("UiButton");
    args.Clear(); args.Set("types",types);
    Check(Ok(host.Execute("describe_controls",args)),"bounded three-schema dialog discovery");
    types.Add("UiPanel"); types.Add("UiTitleCard"); args.Set("types",types);
    Check(!Ok(host.Execute("describe_controls",args)),"schema batch rejects more than four types");
    Value proposal=host.Execute("prepare_composition",example);
    Check(Ok(proposal) && Authored(dialog.Document())==initial,"skill example is schema-valid without mutation");
    auto provider=std::make_shared<AppChatScriptedProvider>(); ValueMap message=AppChatMessage("assistant",""); ValueArray calls;
    for(int i=0;i<2;i++) { ValueMap call,fn; call.Set("id",AsString(i)); call.Set("type","function"); fn.Set("name","inspect_context"); fn.Set("arguments","{}"); call.Set("function",fn); calls.Add(call); }
    message.Set("tool_calls",calls); provider->replies.Add(message);
    AppChatTurn turn; turn.limits.calls=1; turn.Start(provider,ValueArray(),host.Tools()); TimeStop timer;
    while(turn.active && timer.Seconds()<3) { turn.Poll([&](const String& name,const ValueMap& a){return host.Execute(name,a);}); Sleep(1); }
    Check(!turn.error.IsEmpty() && host.Proposals()[0].status=="pending","later read-only budget failure preserves valid proposal");
    int before=dialog.Commands().GetHistoryPosition();
    Check(Ok(host.Apply(ProposalId(proposal))) && dialog.Commands().GetHistoryPosition()==before+1,"dialog example applies as one command");
    bool heading=false,ok=false,cancel=false;
    for(const auto& node:dialog.Document().GetNodes()) { String text=AsString(node.GetProperty("text",""));
        heading|=node.type=="UiLabel" && !text.IsEmpty(); ok|=node.type=="UiButton" && text=="OK"; cancel|=node.type=="UiButton" && text=="Cancel"; }
    Check(heading && ok && cancel,"dialog contains heading and both action buttons");
    Check(dialog.Undo() && Authored(dialog.Document())==initial,"one-step dialog Undo restores exact authored state");
  }
  {
    UiDesignerSession session; UiDesignerAssistantHost host(session);
    auto a = session.AddControl("UiButton"), b = session.AddControl("UiButton");
    session.Select(a); host.Capture("Designer");
    String initial = Authored(session.Document());
    Check(Ok(host.Execute("inspect_context",ValueMap())),"capture context");
    Check(initial == Authored(session.Document()),"discussion/inspection has no mutation");
    ValueMap query; query.Set("query","button"); Check(Ok(host.Execute("search_controls",query)),"catalog search");
    ValueMap skill; skill.Set("id","typography-v1"); Check(Ok(host.Execute("retrieve_skill",skill)),"embedded skill arbitrary cwd");
    ValueMap type; type.Set("type","UiButton"); auto spec = host.Execute("describe_control",type);
    Check(Ok(spec) && spec["result"]["theme_fields"].Is<ValueArray>(),"actual theme field schema");
    Check(ParseJSON(AsJSON(spec)).Is<ValueMap>(),"catalog including typed Color defaults serializes to provider JSON");
    ValueMap inspect; ValueArray inspected; inspected.Add(a); inspect.Set("nodes",inspected);
    Check(ParseJSON(AsJSON(host.Execute("inspect_nodes",inspect))).Is<ValueMap>(),"effective Color values serialize to provider JSON");
    Check(!Ok(host.Execute("commit_property",ValueMap())),"legacy mutation not allowlisted");
    ValueArray edits; edits.Add(Edit(a,"text","First")); edits.Add(Edit(b,"text","Second"));
    ValueMap args = Group(edits); auto p = host.Execute("prepare_edits",args);
    Check(Ok(p),"prepare full batch"); session.Select(b);
    int history = session.Commands().GetHistoryPosition();
    Check(Ok(host.Apply(ProposalId(p))),"human Apply captured nodes");
    Check(session.Document().GetProperty(a,"text") == "First" && session.Document().GetProperty(b,"text") == "Second","selection change did not retarget");
    Check(session.Commands().GetHistoryPosition() == history+1,"one history entry for batch");
    Check(Ok(host.Apply(ProposalId(p))) && session.Commands().GetHistoryPosition() == history+1,"duplicate Apply returns receipt");
    String applied = Authored(session.Document());
    Check(session.Undo() && Authored(session.Document()) == initial,"Undo exact authored document");
    Check(session.Redo() && Authored(session.Document()) == applied,"Redo exact authored document");
    host.Capture("Designer"); history=session.Commands().GetHistoryPosition();
    Value noop=host.Execute("prepare_edits",args);
    Check(Ok(noop) && Ok(host.Apply(ProposalId(noop))) && history==session.Commands().GetHistoryPosition(),"no-op creates no history entry");
    host.Capture("Designer"); edits.Add(Edit(b,"invented_property",true)); history=session.Commands().GetHistoryPosition();
    Check(!Ok(host.Execute("prepare_edits",Group(edits))) && history==session.Commands().GetHistoryPosition() && applied==Authored(session.Document()),"invalid batch no partial state/history");
    edits.Clear(); edits.Add(Edit(a,"text",123)); Check(!Ok(host.Execute("prepare_edits",Group(edits))),"invalid type rejected");
    edits.Clear(); edits.Add(Edit(a,"text","Stale")); args=Group(edits); p=host.Execute("prepare_edits",args);
    session.Commands().SetProperty(b,"text","Manual",UiDesignerImpactControlState);
    Check(!Ok(host.Apply(ProposalId(p))),"document change invalidates proposal");
    host.Capture("Designer"); p=host.Execute("prepare_edits",args); String error;
    session.Theme().Commit("spacing",9,"Manual theme",error);
    Check(!Ok(host.Apply(ProposalId(p))),"Theme change invalidates document proposal");
    host.Capture("Designer"); p=host.Execute("prepare_edits",args); session.Theme().Undo(); session.Theme().Redo();
    Check(!Ok(host.Apply(ProposalId(p))),"Theme undo redo still invalidates token");
    host.Capture("Designer"); p=host.Execute("prepare_edits",args); host.Dismiss(ProposalId(p)); Check(!Ok(host.Apply(ProposalId(p))),"dismiss cannot apply");
    p=host.Execute("prepare_edits",args); host.CancelPending(); Check(!Ok(host.Apply(ProposalId(p))),"cancel cannot apply pending proposal");
    host.Capture("Designer"); ValueArray color_edits; color_edits.Add(Edit(a,"text_normal","#123456","style"));
    auto color_proposal=host.Execute("prepare_edits",Group(color_edits));
    Check(Ok(color_proposal) && Ok(host.Apply(ProposalId(color_proposal))) && session.Document().GetThemeOverride(a,"text_normal") == Color(18,52,86),"wire color becomes typed local style");
    Check(session.Undo(),"typed color Undo");
    host.Capture("Designer"); ValueMap font; font.Set("summary","Replace font"); font.Set("scope","local");
    font.Set("family",Font::GetFaceName(0)); ValueArray nodes; nodes.Add(a); font.Set("nodes",nodes); font.Set("target","");
    session.Commands().SetThemeOverride(a,"font_bold",true,UiDesignerImpactControlState);
    session.Commands().SetThemeOverride(a,"font_italic",true,UiDesignerImpactControlState);
    session.Commands().SetThemeOverride(a,"font_size",23,UiDesignerImpactControlState);
    host.Capture("Designer"); p=host.Execute("prepare_font",font);
    Check(Ok(p) && Ok(host.Apply(ProposalId(p))),"installed font replacement");
    Check(session.Document().GetThemeOverride(a,"font_bold") == true,"font replacement preserves bold");
    Check(session.Document().GetThemeOverride(a,"font_italic") == true && session.Document().GetThemeOverride(a,"font_size") == 23,"font replacement preserves italic and size");
    Check(session.Undo(),"font replacement undo");
    host.Capture("Theme Studio"); font.Set("scope","recipe"); font.Set("nodes",ValueArray()); font.Set("target","Light|control|UiButton|Standard");
    p=host.Execute("prepare_font",font); String document_before=Authored(session.Document());
    Check(Ok(p) && Ok(host.Apply(ProposalId(p))),"explicit Theme recipe font");
    Check(document_before==Authored(session.Document()),"Theme recipe preserves authored controls");
    Check(session.Theme().Get().studio_preview.IsEmpty(),"recipe does not change Studio preview");
    Check(session.Theme().Undo(),"separate Theme Undo");
    Check(Authored(session.Document()).Find("Replace font") < 0 && session.GenerateCode().Find("OPENROUTER_API_KEY") < 0,"transcript and credentials absent from authored/generated output");
    host.Capture("Designer"); ValueMap insert; insert.Set("summary","Shell"); insert.Set("type",session.Catalog().GetPresets()[0].id); insert.Set("preset",true); insert.Set("parent",session.Document().Find(a)->parent);
    p=host.Execute("prepare_insert",insert);
    Check(!Ok(p),"nonempty Window is not replaced by shell");
    {
        UiDesignerSession shell; UiDesignerAssistantHost shell_host(shell); shell_host.Capture("Designer");
        insert.Set("parent",shell.Document().GetRootId());
        Value shell_p=shell_host.Execute("prepare_insert",insert);
        Check(Ok(shell_p) && Ok(shell_host.Apply(ProposalId(shell_p))),"registered preset composition");
        Check(shell.Catalog().ValidateDocument(shell.Document(),error),"shell canonical validation");
    }
    Check(session.Catalog().ValidateDocument(session.Document(),error),"canonical composition validation");
    Check(!session.GenerateCode().IsEmpty(),"composition generation");
    host.Capture("Designer"); insert.Set("preset",false); insert.Set("type","UiTabPage"); insert.Set("parent",a);
    Check(!Ok(host.Execute("prepare_insert",insert)),"invalid semantic parenting rejected");
    p=host.Execute("prepare_edits",args); session.NewDocument(); Check(!Ok(host.Apply(ProposalId(p))),"document switch invalidates proposal");
    Check(!Ok(host.Execute("inspect_context",ValueMap())),"late tools rejected after switch");
    host.Capture("Designer");
    ValueMap composition; composition.Set("summary","Supported settings form"); composition.Set("parent",session.Document().GetRootId());
    ValueArray tree; ValueMap item;
    item.Set("ref","layout"); item.Set("parent_ref",""); item.Set("type","UiBoxLayout"); item.Set("properties",ValueMap()); tree.Add(item);
    item.Set("ref","heading"); item.Set("parent_ref","layout"); item.Set("type","UiLabel");
    ValueMap props; props.Set("text","Settings"); item.Set("properties",props); tree.Add(item);
    item.Set("ref","save"); item.Set("type","UiButton"); props.Set("text","Save settings"); item.Set("properties",props); tree.Add(item);
    composition.Set("items",tree); String empty_document=Authored(session.Document());
    p=host.Execute("prepare_composition",composition); history=session.Commands().GetHistoryPosition();
    Check(Ok(p) && Ok(host.Apply(ProposalId(p))),"typed symbolic subtree composition");
    Check(session.Commands().GetHistoryPosition()==history+1,"composition has one live history entry");
    Check(session.Undo() && Authored(session.Document())==empty_document,"composition undo restores original design");
    host.Capture("Designer"); item.Set("parent_ref","missing"); tree.Set(2,item); composition.Set("items",tree);
    Check(!Ok(host.Execute("prepare_composition",composition)) && Authored(session.Document())==empty_document,"invalid subtree leaves no partial authored state");
    { UiDesignerAssistantDrawer drawer(session); Size authored=session.Document().GetVirtualSize(); drawer.SetRect(0,0,900,320); drawer.Hide(); drawer.Show(); drawer.SetRect(0,0,900,420);
      Check(session.Document().GetVirtualSize()==authored,"drawer collapse and resize preserve authored dimensions"); }
  }
  Cout() << "AssistantDesignerTests checks=" << checks << " failed=" << failed << "\n"; SetExitCode(failed ? 1 : 0);
}
