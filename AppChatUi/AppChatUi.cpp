#include "AppChatUi.h"
namespace Upp {
AppChatMessageCard::AppChatMessageCard() {
    Add(heading);Add(body);Add(status);Add(disclosure);Add(activity_button);Add(activity_view);
    body.SetAlign(UiAlign::LEFT,UiAlign::TOP).SetSelectable();activity_view.SetReadOnly();
    disclosure.WhenAction=[=]{expanded=!expanded;WhenResize();};
    activity_button.SetText("Activity");activity_button.WhenAction=[=]{activity_open=!activity_open;WhenResize();};
    RefreshTheme();
}
void AppChatMessageCard::SetMessage(const String& role,const String& value){heading.SetText(role);SetText(value);}
void AppChatMessageCard::SetText(const String& value){if(text!=value){text=value;measured_width=-1;}}
void AppChatMessageCard::SetActivity(const String& value){if(activity!=value){activity=value;activity_view.SetTextUtf8(value);}}
UiButton& AppChatMessageCard::AddAction(const String& id,const String& label,Event<> callback,UiRole role) {
    action_ids.Add(id);action_roles.Add(role);auto& button=actions.Add();Add(button);button.SetText(label);button.WhenAction=callback;button.SetCustomStyle(UiTheme::ResolveButton(role));return button;
}
void AppChatMessageCard::SetActionState(const String& id,bool visible,bool enabled) {
    int i=FindIndex(action_ids,id);if(i>=0){actions[i].Show(visible);actions[i].Enable(enabled);}
}
int AppChatMessageCard::MeasureAndArrange(int width) {
    heading.SetRect(12,5,max(0,width/2-12),22);status.SetRect(width/2,5,max(0,width/2-12),22);
    // UiLabel supports explicit lines, not width-constrained wrapping. Wrap
    // literal Unicode text here, caching measurement outside Paint.
    if(measured_width!=width) {
        Font font=body.GetStyle().metrics.use_text_font?body.GetStyle().metrics.text_font:body.GetStyle().font;
        if(IsNull(font))font=StdFont();line_height=max(1,GetTextSize("Mg",font).cy);
        WString source=text.ToWString(),line,wrapped;int lines=1,limit=max(1,width-32);
        for(int i=0;i<source.GetCount();i++) {
            wchar c=source[i];if(c=='\r')continue;
            if(c=='\n'){wrapped<<line<<'\n';line.Clear();lines++;continue;}
            line.Cat(c=='\t'?' ':c);
            if(GetTextSize(line,font).cx>limit && line.GetCount()>1) {
                int split=line.ReverseFind(' ');
                if(split<=0)split=line.GetCount()-1;
                wrapped<<line.Left(split)<<'\n';
                line=line.Mid(split+(line[split]==' '?1:0));lines++;
            }
        }
        wrapped<<line;String display=wrapped.ToString();display.Replace("&","&&");
        body.SetText(display);text_height=max(line_height,lines*line_height+4);measured_width=width;
    }
    int full=text_height,height=expanded?full:min(full,line_height*3+4);
    body.SetRect(12,30,max(1,width-24),height);
    int y=34+height,x=12;
    auto place=[&](UiButton& b,int w){if(!b.IsShown())return;if(x+w>width-12&&x>12){x=12;y+=32;}b.SetRect(x,y,w,26);x+=w+5;};
    disclosure.Show(full>line_height*3+4);disclosure.SetText(expanded?"Show less":"Show more");place(disclosure,86);
    for(auto& b:actions)place(b,max(65,min(150,b.GetMinSize().cx+8)));
    activity_button.Show(!activity.IsEmpty());place(activity_button,80);if(x>12)y+=32;
    activity_view.Show(activity_open&&!activity.IsEmpty());
    if(activity_view.IsShown()){activity_view.SetRect(12,y,max(1,width-24),120);y+=126;}
    return y+8;
}
void AppChatMessageCard::RefreshTheme() {
    SetCustomStyle(UiTheme::ResolvePanel(UiPanelRole::Surface));
    disclosure.SetCustomStyle(UiTheme::ResolveButton(UiRole::Subtle));activity_button.SetCustomStyle(UiTheme::ResolveButton(UiRole::Subtle));
    for(int i=0;i<actions.GetCount();++i) actions[i].SetCustomStyle(UiTheme::ResolveButton(action_roles[i]));
    body.ClearCustomStyle();heading.ClearCustomStyle();status.ClearCustomStyle();measured_width=-1;Refresh();
    status.SetCustomStyle(UiTheme::ResolveLabel(status_role));
}
AppChatConversationView::AppChatConversationView(){SetScrollMode(UIPANELSCROLL_VERTICAL);}
AppChatMessageCard& AppChatConversationView::AddMessage(const String& role,const String& text,const String& reference) {
    auto& card=cards.Add();Content().Add(card);card.reference=reference;card.SetMessage(role,text);card.WhenResize=[=]{Arrange();};return card;
}
void AppChatConversationView::ClearMessages(){cards.Clear();SetScrollPos(Point(0,0));Arrange();}
void AppChatConversationView::Arrange(bool latest) {
    if(arranging)return;arranging=true;
    int width=max(100,GetSize().cx-24),y=6;
    for(auto& c:cards){int h=c.MeasureAndArrange(width);c.SetRect(4,y,width,h);y+=h+8;}
    UiScrollPanel::Layout();if(latest)SetScrollPos(Point(0,max(0,y-GetSize().cy)));arranging=false;
}
void AppChatConversationView::Layout(){Arrange();}
void AppChatConversationView::JumpTo(const String& id){for(auto& c:cards)if(c.reference==id){c.SetExpanded(true);Arrange();SetScrollPos(Point(0,c.GetRect().top));return;}}
void AppChatConversationView::FoldMessages(){for(auto& c:cards)c.SetExpanded(false);}
void AppChatConversationView::RefreshTheme(){for(auto& c:cards)c.RefreshTheme();Arrange();}
}
