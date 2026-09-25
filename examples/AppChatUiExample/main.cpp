#include <AppChatUi/AppChatUi.h>
using namespace Upp;
GUI_APP_MAIN {
    TopWindow window;AppChatConversationView conversation;
    window.Title("AppChatUi - presentation example").Sizeable().SetRect(0,0,720,420);
    window.Add(conversation.SizePos());
    conversation.AddMessage("You","Prepare an example change.");
    auto& reply=conversation.AddMessage("Assistant","This card uses ordinary Ui controls. The application owns the change, validates it and provides the action callback. Expanding or navigating a card never executes an action.","example-1");
    reply.SetStatus("Ready");
    reply.AddAction("review","Review",[&]{reply.SetStatus("Reviewed by example host");});
    reply.SetActivity("Local example: no model, credentials, network, document edits or Designer dependencies.");
    window.Run();
}
