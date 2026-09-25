#ifndef _UiDesigner_Assistant_h_
#define _UiDesigner_Assistant_h_
#include <AppChat/AppChat.h>
#include <UiDesigner/Services/UiDesignerAutomation.h>
namespace Upp {
struct UiDesignerAssistantProposal {
    String id, summary, kind, status = "pending", receipt;
    uint64 generation = 0, revision = 0;
    String theme_token;
    ValueMap args;
    Vector<UiDesignerAuthoredEdit> edits;
    Vector<UiDesignerNodeId> affected;
};
class UiDesignerAssistantHost {
    UiDesignerSession& session;
    UiDesignerAutomationService automation;
    uint64 generation = 0, revision = 0;
    String theme_token;
    ValueMap captured;
    Array<UiDesignerAssistantProposal> proposals;
    bool ValidateEdits(const ValueArray&, Vector<UiDesignerAuthoredEdit>&, String&) const;
    bool Composition(const ValueMap&, UiDesignerDocument&, Vector<UiDesignerNodeId>&, String&) const;
    Value Prepare(const String&, const ValueMap&);
    Value ExecuteOperation(const String&, const ValueMap&);
    bool Current(uint64, uint64, const String&) const;
public:
    explicit UiDesignerAssistantHost(UiDesignerSession& s) : session(s), automation(s) {}
    ValueMap Capture(const String& workspace);
    ValueArray Tools() const;
    Value Execute(const String&, const ValueMap&);
    Value Apply(const String& id); // trusted UI only; absent from Tools/Execute
    Value ApplyPending(); // explicit human text command; never a model tool
    void ClearConversation();
    void SupersedePending(const String& id);
    String ProposalState(const String& id) const;
    Value RefinementContext(const String& id) const;
    void Dismiss(const String& id);
    void CancelPending();
    void ShowAffected(const String& id);
    const Array<UiDesignerAssistantProposal>& Proposals() const { return proposals; }
    bool SameDocument() const { return generation == session.GetDocumentGeneration(); }
    String SystemPrompt() const;
};
}
#endif
