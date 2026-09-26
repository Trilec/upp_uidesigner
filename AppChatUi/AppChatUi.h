#ifndef _AppChatUi_h_
#define _AppChatUi_h_
#include <Ui/Ui.h>
namespace Upp {
// Presentation only. Applications own messages/proposals and supply stable IDs
// and action callbacks. This package never applies edits or calls a provider.
class AppChatMessageCard : public UiPanel {
    UiLabel heading, body, status;
    UiButton disclosure, activity_button;
    UiMultiEdit activity_view;
    Array<UiButton> actions;
    Vector<String> action_ids;
    Vector<UiRole> action_roles;
    String text, activity;
    UiRole status_role=UiRole::Standard;
    bool expanded=false, activity_open=false;
    int measured_width=-1, text_height=20, line_height=20;
public:
    String reference;
    Event<> WhenResize;
    AppChatMessageCard();
    void SetMessage(const String& role,const String& text);
    void SetText(const String& value);
    const String& GetText() const {return text;}
    void SetStatus(const String& value){status.SetText(value);}
    void SetStatusRole(UiRole role){status_role=role;status.SetCustomStyle(UiTheme::ResolveLabel(role));}
    void SetActivity(const String& value);
    void SetExpanded(bool value){expanded=value;}
    UiButton& AddAction(const String& id,const String& label,Event<> callback,UiRole role=UiRole::Standard);
    void SetActionState(const String& id,bool visible,bool enabled=true);
    int MeasureAndArrange(int width);
    void RefreshTheme();
};
class AppChatConversationView : public UiScrollPanel {
    Array<AppChatMessageCard> cards;
    bool arranging=false;
public:
    AppChatConversationView();
    AppChatMessageCard& AddMessage(const String& role,const String& text,const String& reference=String());
    int GetCount() const{return cards.GetCount();}
    AppChatMessageCard& At(int i){return cards[i];}
    void ClearMessages();
    void Arrange(bool latest=false);
    void JumpTo(const String& reference);
    void FoldMessages();
    void RefreshTheme();
    void Layout() override;
};
}
#endif
