#ifndef _UiDesigner_AssistantDrawer_h_
#define _UiDesigner_AssistantDrawer_h_
#include <Ui/Ui.h>
#include <UiDesigner/Assistant/UiDesignerAssistant.h>
namespace Upp {
class UiDesignerAssistantComposer : public UiMultiEdit {
public:
    Event<> WhenSend;
    bool Key(dword key, int count) override {
        if(key == K_ENTER) { WhenSend(); return true; }
        if(key == (K_SHIFT | K_ENTER)) return UiMultiEdit::Key(K_ENTER, count);
        return UiMultiEdit::Key(key, count);
    }
};
class UiDesignerAssistantDrawer : public UiPanel {
    UiDesignerAssistantHost host;
    AppChatTurn turn;
    UiLabel context, profile_label;
    UiDropdown provider, proposals;
    UiLineEdit model, credential;
    UiMultiEdit transcript, proposal_detail;
    UiDesignerAssistantComposer composer;
    UiButton send, stop, collapse, profile_toggle, configure, apply, dismiss, affected;
    AppChatProfile profile;
    ValueArray conversation;
    String history, submitted, last_projection;
    bool was_active = false, configured = false, profile_open = false;
    int proposal_count = 0, drag_y = 0, initial_height = 0;
    void Submit();
    void Tick();
    void UpdateProposal();
    void Configure();
    void SyncProfileSummary();
public:
    explicit UiDesignerAssistantDrawer(UiDesignerSession&);
    ~UiDesignerAssistantDrawer();
    Event<> WhenCollapse;
    Event<int> WhenHeight;
    Function<String()> Workspace;
    void Stop();
    void RefreshTheme();
    void Layout() override;
    void Paint(Draw&) override;
    void LeftDown(Point, dword) override;
    void MouseMove(Point, dword) override;
    void LeftUp(Point, dword) override;
};
}
#endif
