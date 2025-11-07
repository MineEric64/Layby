#pragma once
#define NOMINMAX

#ifdef CEFWRAPPER_EXPORTS
#define CEFWRAPPER_API __declspec(dllexport)
#else
#define CEFWRAPPER_API __declspec(dllimport)
#endif

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/wrapper/cef_helpers.h"

class PlayerHandler : public CefRenderHandler, public CefAudioHandler {
public:
    //Video
    PlayerHandler() {}
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer, int width, int height) override;

    //Audio
    bool GetAudioParameters(CefRefPtr<CefBrowser> browser, CefAudioParameters& params) override;
    void OnAudioStreamStarted(CefRefPtr<CefBrowser> browser, const CefAudioParameters& params, int channel) override;
    void OnAudioStreamPacket(CefRefPtr<CefBrowser> browser, const float** data, int frames, long long pts) override;
    void OnAudioStreamStopped(CefRefPtr<CefBrowser> browser) override;
    void OnAudioStreamError(CefRefPtr<CefBrowser> browser, const CefString& message) override;

private:
    IMPLEMENT_REFCOUNTING(PlayerHandler);
};

class PlayerBrowserClient : public CefClient, CefRequestHandler {
public:
    PlayerBrowserClient(CefRefPtr<PlayerHandler> handler_) : handler(handler_) {}
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return handler; }
    virtual CefRefPtr<CefAudioHandler> GetAudioHandler() { return handler; }

    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
    virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) {
        CefRequest::HeaderMap hdrMap;

        request->GetHeaderMap(hdrMap);
        if (hdrMap.find("Referrer-Policy") != hdrMap.end()) hdrMap.erase("Referrer-Policy");
        hdrMap.insert(std::make_pair("Referrer-Policy", "strict-origin-when-cross-origin"));
        request->SetHeaderMap(hdrMap);

        return false;
    }

private:
    CefRefPtr<PlayerHandler> handler;
    IMPLEMENT_REFCOUNTING(PlayerBrowserClient);
};

class PlayerApp : public CefApp, public CefBrowserProcessHandler {
public:
    PlayerApp() {}
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    void OnContextInitialized() {}

private:
    IMPLEMENT_REFCOUNTING(PlayerApp);
};

extern "C" {
    CEFWRAPPER_API int initializeCEF(const char* path, const wchar_t* cache);
    CEFWRAPPER_API void shutdownCEF();
    CEFWRAPPER_API int isInitialized();

    CEFWRAPPER_API void resized();
    CEFWRAPPER_API void loadURL(const char* url);
    CEFWRAPPER_API void getURL(char* url);
    CEFWRAPPER_API void timerCallback();
    CEFWRAPPER_API void setLocalBounds(int width_, int height_);
    CEFWRAPPER_API void setAudioParam(int sampleRate_, int samplesPerBlock_);

    CEFWRAPPER_API void mouseMove(int x, int y);
    CEFWRAPPER_API void mouseDown(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
    CEFWRAPPER_API void mouseUp(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
    CEFWRAPPER_API int keyPressed(const char16_t key, int vk);
    CEFWRAPPER_API void executeJS(const char* script);
    CEFWRAPPER_API void showDevTools();

    CEFWRAPPER_API void getImageSize(int* width, int* height);
    CEFWRAPPER_API int getImage(void* p, int length);
    CEFWRAPPER_API int getAudioBuffer(float** data, int length, int destChannels);
}