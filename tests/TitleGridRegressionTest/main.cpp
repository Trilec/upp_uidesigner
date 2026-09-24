#include <CtrlLib/CtrlLib.h>
#include <UiDesigner/Services/UiDesignerServices.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Preview/UiDesignerSelectionHit.h>
using namespace Upp;
static int checks=0, failed=0;
static void Check(bool ok, const char* label) { ++checks; if(!ok) ++failed; Cout()<<(ok?"PASS ":"FAIL ")<<label<<'\n'; }
static Rect LinePixels(UiTitleCard& card) {
    ImageDraw draw(card.GetSize()); draw.DrawRect(card.GetSize(), White()); card.Paint(draw);
    Image image=draw; Rect bounds;
    bool found=false;
    for(int y=0;y<image.GetHeight();y++) for(int x=0;x<image.GetWidth();x++) {
        const RGBA& p=image[y][x];
        if(p.r==255 && p.g==0 && p.b==255) {
            if(!found) { bounds=RectC(x,y,1,1); found=true; }
            else bounds.Union(RectC(x,y,1,1));
        }
    }
    return bounds;
}
GUI_APP_MAIN {
  {
    UiTitleCard card; card.SetTitle("A complete heading").SetRect(0,0,500,160);
    card.ShowTitleLine().ShowCardLine(false).SetTitleLine(MEDIUM,2,SOLID,Color(255,0,255));
    Check(LinePixels(card).GetWidth()==GetTextSize("A complete heading",card.GetStyle().title_font).cx,"Medium title line measures full title");
    card.ShowTitleLine(false).ShowCardLine().SetCardLine(MEDIUM,2,SOLID,Color(255,0,255));
    Check(LinePixels(card).GetWidth()==GetTextSize("A complete heading",card.GetStyle().title_font).cx,"Medium card line measures full title");
    UiButton extra; extra.SetText("Extra"); card.SetContentCell(extra).SetContentCellGap(12);
    card.SetCardLine(LARGE,2,SOLID,Color(255,0,255)); card.Layout();
    Rect line=LinePixels(card);
    Check(line.GetWidth()>0 && line.right<=extra.GetRect().left-12,"Large card line stops before hosted control gap");
    UiGridLayout grid; UiTitleCard heading; UiPanel panel; UiBoxLayout actions; UiButton ok,cancel;
    heading.SetTitle("Dialog heading"); ok.SetText("OK"); cancel.SetText("Cancel");
    actions.SetDirection(UiDirection::H); actions.Add(ok).Fit(); actions.Add(cancel).Fit();
    grid.SetGridSize(1,3).SetMinCellSize(Size(1,1));
    grid.Add(heading,0,0,true,false); grid.Add(panel,1,0,true,true); grid.Add(actions,2,0,true,false);
    Size natural=grid.GetMinSize();
    Check(natural.cy>=heading.GetMinSize().cy+actions.GetMinSize().cy,"Grid Fit measures descendants without collapsing");
    Check(grid.MeasureHeightForWidth(500)>=heading.GetMinSize().cy+actions.GetMinSize().cy,"Grid width-constrained height retains natural rows");
    grid.SetRect(0,0,500,600); grid.Layout();
    Check(grid.GetCellRect(2,0).GetHeight()==actions.GetMinSize().cy,"Fit action row consumes only button content height");
    Check(grid.GetCellRect(1,0).GetHeight()>grid.GetCellRect(2,0).GetHeight(),"Expand panel row receives remaining height");
    grid.SetItem(0,0,0,true,false,Size(0,91)); grid.Layout();
    Check(grid.GetCellRect(0,0).GetHeight()==91,"Single-axis Fixed height respected");
    actions.SetWrap(true);
    grid.SetItem(2,2,0,false,false,Size(170,83)); grid.Layout();
    Check(grid.GetCellRect(2,0).GetHeight()==83 && actions.GetSize().cx==170,"Wrapped content respects both fixed dimensions");
  }
  {
    UiDesignerSession session; UiDesignerPreviewCanvas preview; preview.SetRect(0,0,600,600); session.AttachProjection(&preview);
    auto grid=session.AddControl("UiGridLayout"); session.Select(grid); String error;
    session.CommitProperty("rows",3,error); session.CommitProperty("columns",1,error);
    auto add=[&](const char* type,UiDesignerNodeId parent,int row) {
        UiDesignerNodeId id=0; auto plan=session.PlanAddControl(type,parent,Point(),false,-1,row,0);
        Check(plan.valid && session.ExecuteDrop(plan,&id,error),"fixture child insertion"); return id;
    };
    auto title=add("UiTitleCard",grid,0), panel=add("UiPanel",grid,1), box=add("UiBoxLayout",grid,2);
    add("UiButton",box,0); add("UiButton",box,0);
    auto set=[&](UiDesignerNodeId id,const char* key,const Value& value) { session.Select(id); Check(session.CommitProperty(key,value,error),key); };
    set(title,"height_mode","Fit"); set(panel,"height_mode","Expand"); set(box,"height_mode","Fit");
    set(box,"direction","H"); set(title,"role","Accent");
    auto* card=dynamic_cast<UiTitleCard*>(preview.FindRuntime(title));
    Check(card && card->GetStyle().title_color==UiTheme::ResolveTitleCard(UiRole::Accent).title_color,"Preview retains Accent without local overrides");
    set(title,"title_line_length","Medium"); set(title,"card_line_length","Medium");
    card=dynamic_cast<UiTitleCard*>(preview.FindRuntime(title));
    Check(card && card->GetStyle().title_line_length==MEDIUM,"Inspector Medium reaches runtime");
    auto* runtime=dynamic_cast<UiGridLayout*>(preview.FindRuntime(grid)); preview.Layout();
    Check(runtime && runtime->GetCellRect(2,0).GetHeight()<runtime->GetCellRect(1,0).GetHeight(),"Designer Fit child shrinks its row");
    set(grid,"height_mode","Fit"); preview.Layout();
    Check(preview.FindRuntime(grid)->GetSize().cy>=card->GetMinSize().cy,"Designer Grid Fit remains measurable");
    auto stack=UiDesignerPreviewSelectionStack(preview.GetGeometrySnapshot(),session.Document(),preview.FindGeometry(title)->rect.CenterPoint());
    Check(FindIndex(stack,grid)>=0,"Covered Grid remains in ancestor click-selection stack");
    String code=session.GenerateCode();
    Check(code.Find("UiTheme::ResolveTitleCard(UiRole::Accent)")>=0,"Export retains TitleCard role");
    Check(code.Find(".AddGrid(")>=0 && code.Find("MEDIUM")>=0,"Export emits Grid item sizing and Medium lines");
    session.AttachProjection(nullptr);
  }
  Cout()<<"TitleGridRegressionTest checks="<<checks<<" failed="<<failed<<'\n'; SetExitCode(failed?1:0);
}
