#include "AppChat.h"
#ifdef PLATFORM_WIN32
#include <winhttp.h>
#endif
namespace Upp {
AppChatReply AppChatDeepSeekProvider::Complete(const ValueArray& messages, const ValueArray& tools,
    const AppChatLimits& limits, std::atomic<bool>& cancel, const Function<void(String)>& visible) {
    AppChatReply result;
    if(!profile.Validate(result.error)) return result;
#ifdef PLATFORM_WIN32
    struct Handle { HINTERNET h = nullptr; ~Handle() { if(h) WinHttpCloseHandle(h); } } session, connection, request;
    ValueMap body; body.Set("model", profile.model); body.Set("messages", messages);
    body.Set("tools", tools); body.Set("stream", true); body.Set("max_tokens", limits.tokens);
    bool router = profile.provider == "OpenRouter";
    if(!router) { ValueMap thinking; thinking.Set("type", "disabled"); body.Set("thinking", thinking); }
    else { ValueMap reasoning; reasoning.Set("enabled", false); body.Set("reasoning", reasoning);
        ValueMap routing; routing.Set("require_parameters", true); body.Set("provider", routing); }
    String json = AsJSON(body);
    if(json.GetCount() > limits.request_bytes) { result.error = "Request limit exhausted"; return result; }
    session.h = WinHttpOpen(L"UiDesignerAssistant/1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if(!session.h) { result.error = "Unable to initialize HTTPS"; return result; }
    WinHttpSetTimeouts(session.h, 5000, 5000, 5000, 15000);
    connection.h = WinHttpConnect(session.h, router ? L"openrouter.ai" : L"api.deepseek.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if(connection.h) request.h = WinHttpOpenRequest(connection.h, L"POST", router ? L"/api/v1/chat/completions" : L"/chat/completions",
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if(!request.h) { result.error = "Unable to create HTTPS request"; return result; }
    DWORD redirects = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    if(!WinHttpSetOption(request.h, WINHTTP_OPTION_REDIRECT_POLICY, &redirects, sizeof(redirects))) {
        result.error = "Unable to disable credential-bearing redirects"; return result;
    }
    // U++ WString is UTF-32; WinHTTP requires Windows UTF-16.
    Vector<char16> headers = ToSystemCharsetW("Content-Type: application/json\r\nAuthorization: Bearer " + GetEnv(profile.credential_env) + "\r\n");
    if(cancel) { result.error = "Cancelled"; return result; }
    TimeStop elapsed;
    if(!WinHttpSendRequest(request.h, headers.begin(), headers.GetCount() - 1,
        (void*)~json, json.GetCount(), json.GetCount(), 0) || !WinHttpReceiveResponse(request.h, nullptr)) {
        DWORD code = GetLastError();
        result.error = cancel ? "Cancelled" : "HTTPS connection, authentication handshake or timeout failure (Windows " + AsString((int)code) + ")"; return result;
    }
    DWORD status = 0, n = sizeof(status);
    WinHttpQueryHeaders(request.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &n, WINHTTP_NO_HEADER_INDEX);
    if(status != 200) {
        result.error = status == 401 ? "Provider authentication failed (401)" : status == 429
            ? "Provider rate limit (429); retry manually" : "Provider HTTP error " + AsString((int)status);
        return result;
    }
    AppChatStream stream;
    for(;;) {
        if(cancel) { result.error = "Cancelled"; return result; }
        if(elapsed.Elapsed() > (double)limits.timeout_ms * 1000) { result.error = "Request timeout"; return result; }
        char buffer[4096]; DWORD read = 0;
        if(!WinHttpReadData(request.h, buffer, sizeof(buffer), &read)) {
            result.error = "Stream disconnected or timed out"; return result;
        }
        if(!read) break;
        if(!stream.Feed(String(buffer, read), limits.output_bytes)) break;
        visible(stream.Text());
    }
    return stream.Finish();
#else
    result.error = "DeepSeek HTTPS transport requires Windows"; return result;
#endif
}
}
