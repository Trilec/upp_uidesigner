#ifndef _AppChat_AppChat_h_
#define _AppChat_AppChat_h_
#include <Core/Core.h>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>
namespace Upp {
struct AppChatLimits {
    // SSE repeats provider metadata for each token; this bounds wire bytes,
    // while tokens and request_bytes separately bound generated text/continuation.
    int rounds = 6, calls = 16, output_bytes = 2097152;
    int request_bytes = 524288, timeout_ms = 60000, tokens = 4096;
};
struct AppChatProfile {
    String provider = "DeepSeek", endpoint, model, credential_env;
    bool Validate(String& error) const;
};
struct AppChatReply {
    ValueMap message;
    String error;
};
class AppChatProvider {
public:
    virtual ~AppChatProvider() {}
    virtual AppChatReply Complete(const ValueArray& messages, const ValueArray& tools,
        const AppChatLimits& limits, std::atomic<bool>& cancel,
        const Function<void(String)>& visible) = 0;
};
class AppChatScriptedProvider : public AppChatProvider {
public:
    ValueArray replies;
    int position = 0;
    AppChatReply Complete(const ValueArray&, const ValueArray&, const AppChatLimits&,
        std::atomic<bool>&, const Function<void(String)>&) override;
};
class AppChatDeepSeekProvider : public AppChatProvider {
    AppChatProfile profile;
public:
    explicit AppChatDeepSeekProvider(const AppChatProfile& p) : profile(p) {}
    AppChatReply Complete(const ValueArray&, const ValueArray&, const AppChatLimits&,
        std::atomic<bool>&, const Function<void(String)>&) override;
};
// Incremental SSE parser. Only Finish exposes executable complete tool arguments.
class AppChatStream {
    String pending, content, reasoning, finish;
    ValueArray calls, reasoning_details;
    int bytes = 0;
    bool done = false;
public:
    String error;
    bool Feed(const String& chunk, int limit);
    String Text() const { return content; }
    AppChatReply Finish();
};
// Poll on the host thread. Worker captures a shared mailbox, never a host/UI pointer.
class AppChatTurn {
    struct Mailbox {
        std::atomic<bool> cancel{false}, done{false};
        std::mutex mutex;
        String visible;
        AppChatReply reply;
    };
    std::shared_ptr<Mailbox> mailbox;
    std::shared_ptr<AppChatProvider> provider;
    std::thread worker;
    ValueArray messages, descriptors;
    int round = 0, calls = 0;
    void Launch();
public:
    AppChatLimits limits;
    Event<const String&> WhenActivity;
    int GetRound() const { return round; }
    int GetCallsUsed() const { return calls; }
    String text, error;
    bool active = false;
    ~AppChatTurn();
    bool Start(std::shared_ptr<AppChatProvider>, const ValueArray&, const ValueArray&);
    void Poll(const Function<Value(const String&, const ValueMap&)>& execute);
    void Stop();
};
ValueMap AppChatMessage(const String& role, const String& content);
}
#endif
