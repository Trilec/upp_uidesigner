#include <UiDesigner/AssistantUi/UiDesignerAssistantDrawer.h>
#include <UiDesigner/Theme/UiDesignerThemeBuilderV2.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>
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
    UiDesignerSession session;
    const auto* spec = session.Catalog().Find("UiProgressBar");
    const auto* adapter = UiDesignerGetThemeAdapter(*spec);
    UiDesignerNode node; node.type = "UiProgressBar"; node.theme_overrides.Set("ink_normal", Color(12,34,56));
    UiProgressBar progress; adapter->ApplyPreviewStyle(progress,node,*spec,nullptr);
    String code; adapter->EmitSetup(code,"progress",node,*spec);
    Check(progress.GetStyle().fill_palette.ink[ST_NORMAL] == Color(12,34,56) &&
          progress.GetStyle().filled_text == Color(12,34,56) &&
          adapter->ResolveFieldValue(node,*spec,"ink_normal",nullptr) == Color(12,34,56) &&
          code.Find("fill_palette.ink[ST_NORMAL]") >= 0,
          "progress ink alias agrees across preview, inspection and export");
  }
  {
    UiDesignerSession session; UiDesignerAssistantHost host(session); host.Capture("theme");
    ValueMap edit = ParseJSON(R"json({"summary":"Solid yellow surface","target":"Light|control|UiButton|Accent","fields":{"face_normal":{"schema":1,"mode":"Solid","solid":"#FFD43B"}}})json");
    Value proposal = host.Execute("prepare_theme",edit);
    Check(Ok(proposal) && Ok(host.Apply(ProposalId(proposal))), "solid fill refinement uses typed colour decoding");
    ValueMap fields = edit["fields"], fill = fields["face_normal"];
    fill.Set("mode","Unknown"); fields.Set("face_normal",fill); edit.Set("fields",fields);
    host.Capture("theme");
    Check(!Ok(host.Execute("prepare_theme",edit)), "unsupported fill mode is rejected before proposal");
  }
  {
    UiDesignerSession session; UiDesignerAssistantHost host(session); host.Capture("theme");
    String original = session.Theme().Serialize(false), document = Authored(session.Document()), error;
    uint64 revision = session.Theme().GetRevision();
    Check(Ok(host.Execute("inspect_theme", ValueMap())), "palette inspection needs no invented recipe target");
    ValueMap lookup; lookup.Set("type", "Accordion"); lookup.Set("query", "title");
    Value discovered = host.Execute("inspect_theme_control", lookup);
    Check(Ok(discovered) && discovered["result"]["type"] == "UiAccordion" &&
          ((ValueArray)discovered["result"]["fields"]).GetCount() > 0,
          "friendly control name resolves bounded theme fields and exact targets");
    ValueMap design = ParseJSON(R"json({"summary":"Yellow brutalist baseline","light":["#F5F2E8","#FFFFFF","#292929","#111111","#E5B900","#BF263C"],"dark":["#151515","#222222","#777777","#F4F4F4","#F7D540","#FF7788"],"radius":0,"border_width":2,"replace_authored":false})json");
    ValueMap style;
    style.Set("body_size",16.0); style.Set("heading_size",22.0);
    style.Set("body_bold",false); style.Set("heading_bold",true);
    style.Set("shadow","Hard"); style.Set("shadow_offset",4.0); style.Set("shadow_alpha",255);
    style.Set("line_width",3); design.Set("style",style);
    Value proposal = host.Execute("prepare_theme_design", design);
    Check(Ok(proposal), "whole theme preparation accepts bounded palette contract");
    try {
        UiDesignerThemeGalleryV2 gallery;
        gallery.SetCatalog(&session.Catalog());
        gallery.SetThemeDocument(&session.Theme());
        gallery.SetRect(0,0,1100,850); gallery.Layout(); gallery.RefreshTheme();
        Check(true, "complete gallery consumes generated recipes");
    } catch(const ValueTypeError& e) {
        Cout() << "Gallery value error: " << e << '\n';
        Check(false, "complete gallery consumes generated recipes");
    } catch(...) { Check(false, "complete gallery consumes generated recipes"); }
    Check(session.Theme().HasProposal() && session.Theme().IsProposalVisible(), "theme proposal is previewed");
    Check(session.Theme().Serialize(false)==original && session.Theme().GetRevision()==revision && !session.Theme().CanUndo() && Authored(session.Document())==document,
          "preview changes neither durable Theme, Document nor undo history");
    String button_target="Light|control|UiButton|Accent";
    Check(session.Theme().GetEffective().GetStyleOverride(button_target,"font_size")==16 &&
          session.Theme().GetEffective().GetStyleOverride("Light|panel|UiGroupPanel|Accent","title_font_height")==22,
          "theme design distinguishes body and heading typography");
    Check(session.Theme().GetEffective().GetStyleOverride(button_target,"shadow_mode")=="Hard" &&
          session.Theme().GetEffective().GetStyleOverride(button_target,"shadow_offset_x")==4,
          "theme design generates hard offset surface shadows");
    Check(session.Theme().GetEffective().GetStyleOverride(button_target,"radius")==0,
          "baseline generates explicit editable adapter geometry");
    Value button_before=session.Theme().GetEffective().GetStyleOverrides(button_target);
    session.Theme().SetActiveStyleTarget("Light|control|UiAccordion|Accent");
    Check(session.Theme().IsProposalVisible(), "sample selection retains proposed theme");
    session.Theme().ShowProposal(false);
    Check(session.Theme().GetEffective().ToValue()==session.Theme().Get().ToValue(), "Compare shows original without discarding candidate");
    session.Theme().ShowProposal(true);
    host.Capture("theme");
    ValueMap refine; refine.Set("summary","Lighter accordion title"); refine.Set("target","Light|control|UiAccordion|Accent");
    ValueMap fields; fields.Set("header_title_color","#626262"); refine.Set("fields",fields);
    Value refined=host.Execute("prepare_theme",refine);
    Check(Ok(refined), "targeted recipe refines the pending theme");
    Check(session.Theme().GetEffective().GetStyleOverrides(button_target)==button_before,
          "refinement preserves unrelated button fields");
    Check(session.Theme().Serialize(false)==original, "refinement stays outside durable theme");
    Check(session.Theme().Commit("palette.light.4", Color(40,120,180), "Adjust proposed accent", error) &&
          session.Theme().GetEffective().GetStyleOverride(button_target,"frame_normal") == Color(40,120,180),
          "palette adjustment regenerates owned fields in the candidate");
    Check(session.Theme().GetEffective().GetStyleOverride("Light|control|UiAccordion|Accent","header_title_color") == Color(98,98,98),
          "palette regeneration preserves the explicit title refinement");
    Check(session.Theme().GetEffective().GetStyleOverride(button_target,"font_size")==16 &&
          session.Theme().GetEffective().GetStyleOverride(button_target,"shadow_offset_x")==4,
          "palette refinement retains generated typography and elevation");
    Check(!Ok(host.Execute("describe_control",ValueMap() )) && session.Theme().HasProposal(),
          "later read failure retains a valid proposal");
    Check(Ok(host.Apply(ProposalId(refined))), "human Keep accepts the candidate");
    Check(session.Theme().GetRevision()==revision+1 && !session.Theme().HasProposal(), "Keep makes one Theme history change");
    String kept=session.Theme().Serialize(false);
    SaveFile(AppendFileName(GetFileFolder(GetExeFilePath()), "yellow-theme-example.theme.json"), kept);
    Check(session.Theme().Undo() && session.Theme().Serialize(false)==original && session.Theme().Redo() && session.Theme().Serialize(false)==kept,
          "one-step Theme Undo/Redo restores exact snapshots");
    String file=AppendFileName(GetTempPath(),"uidesigner-theme-test-"+AsString(Uuid::Create())+".json");
    Check(session.SaveThemeFile(file,error) && !session.IsThemeFileDirty() && session.Theme().IsDirty(),
          "standalone Save does not mark the project Theme checkpoint saved");
    UiDesignerSession loaded;
    Check(loaded.LoadThemeFile(file,error) && loaded.Theme().Serialize(false)==kept, "saved theme reloads identically");
    FileDelete(file);
    host.Capture("theme");
    design.Set("light",ValueArray());
    Check(!Ok(host.Execute("prepare_theme_design",design)) && !session.Theme().HasProposal(), "invalid palette cannot stage partial theme");
  }
  {
    UiDesignerSession session;UiDesignerAssistantHost host(session);host.Capture("Designer");
    ValueMap query;query.Set("id","layout-v2");Value skill=host.Execute("retrieve_skill",query);
    Value proposal=host.Execute("prepare_composition",skill["result"]["grid_label_dialog_example"]);
    String initial=Authored(session.Document());
    Check(Ok(proposal) && Ok(host.Apply(ProposalId(proposal))),"Grid Label dialog guidance is executable");
    bool grid=false,label=false,body=false,spacer=false;
    for(const auto& n:session.Document().GetNodes()) {
        grid|=n.type=="UiGridLayout" && n.GetProperty("rows",0)==3;
        label|=n.type=="UiLabel" && n.GetProperty("grid_row",-1)==0 && n.GetProperty("height_mode","")=="Fit";
        body|=n.type=="UiPanel" && n.GetProperty("grid_row",-1)==1 && n.GetProperty("height_mode","")=="Expand";
        spacer|=n.type=="Spacer" && n.GetProperty("h_sizing","")=="Fill";
    }
    Check(grid&&label&&body&&spacer,"outside-in dialog has Fit heading, expanding body and action spacer");
    Check(session.Undo() && Authored(session.Document())==initial,"Grid example remains one Undo");
    host.Capture("Designer");ValueMap preset;preset.Set("summary","Dialog preset");preset.Set("preset",true);
    preset.Set("type","DialogTemplate");preset.Set("parent",session.Document().GetRootId());
    proposal=host.Execute("prepare_insert",preset);
    Check(Ok(proposal) && Ok(host.Apply(ProposalId(proposal))) && session.Document().GetCount()==8,"insertable dialog preset uses ordinary seven-node composition");
    String error;Check(session.Catalog().ValidateDocument(session.Document(),error) && !session.GenerateCode().IsEmpty() && session.Undo(),"dialog preset validates, generates and undoes");
    host.Capture("Designer");
    ValueMap nested=ParseJSON(AsJSON(skill["result"]["titlecard_dialog_example"]));
    ValueArray items=nested["items"];
    ValueMap slot,props;slot.Set("ref","header_actions");slot.Set("parent_ref","heading");slot.Set("type","UiBoxLayout");
    props.Set("direction","H");props.Set("wrap","Flow");props.Set("width_mode","Expand");props.Set("height_mode","Fit");slot.Set("properties",props);items.Add(slot);
    for(int i=0;i<2;i++){ValueMap button,p;p.Set("text",i?"Export":"Save");button.Set("ref",i?"export":"save");button.Set("parent_ref","header_actions");button.Set("type","UiButton");button.Set("properties",p);items.Add(button);}
    nested.Set("items",items);proposal=host.Execute("prepare_composition",nested);
    Check(Ok(proposal) && Ok(host.Apply(ProposalId(proposal))) && session.Undo(),"TitleCard single slot accepts wrapping Box with multiple buttons");
    host.Capture("Designer");ValueMap direct=items[items.GetCount()-1];direct.Set("parent_ref","heading");items.Set(items.GetCount()-1,direct);nested.Set("items",items);
    Check(!Ok(host.Execute("prepare_composition",nested)),"TitleCard rejects second direct content child");
  }
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
