#ifndef _UiDesigner_AssistantDrawer_h_
#define _UiDesigner_AssistantDrawer_h_
#include <AppChatUi/AppChatUi.h>
#include <UiDesigner/Assistant/UiDesignerAssistant.h>
namespace Upp {
class UiDesignerAssistantComposer : public UiMultiEdit {
public:
    Event<> WhenSend;
    bool Key(dword key,int count) override {
        if(key==K_ENTER){WhenSend();return true;}
        if(key==(K_SHIFT|K_ENTER))return UiMultiEdit::Key(K_ENTER,count);
        return UiMultiEdit::Key(key,count);
    }
};
class UiDesignerAssistantDrawer : public UiPanel {
    UiDesignerSession& session;
    UiDesignerAssistantHost host;
    AppChatTurn turn;
    AppChatConversationView transcript;
    UiLabel heading,context,reference;
    UiDropdown history;
    UiDesignerAssistantComposer composer;
    UiButton send,profile_button,clear,undo,clear_reference;
    AppChatProfile profile;
    ValueArray conversation;
    String refinement_id,submitted_refinement,history_signature,activity;
    bool was_active=false,configured=false;
    int active_message=-1,proposal_count=0,drag_y=0,initial_height=0;
    void Submit();
    void Tick();
    void SyncProposals();
    void Configure();
    void ShowCode(const String& id);
    void Refine(const String& id);
public:
    explicit UiDesignerAssistantDrawer(UiDesignerSession&);
    ~UiDesignerAssistantDrawer();
    Event<int> WhenHeight;
    Function<String()> Workspace;
    void Stop();
    void ClearConversation();
    void RefreshTheme();
    void Layout() override;
    void Paint(Draw&) override;
    void LeftDown(Point,dword) override;
    void MouseMove(Point,dword) override;
    void LeftUp(Point,dword) override;
};
}
#endif
