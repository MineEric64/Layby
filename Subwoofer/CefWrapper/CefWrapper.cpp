#include <queue>
#include <string>
#include "CefWrapper.h"

#pragma comment(lib, "libcef.dll")
#pragma comment(lib, "libcef_dll_wrapper.dll")
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

class PlayerBrowserClient : public CefClient {
public:
    PlayerBrowserClient(CefRefPtr<PlayerHandler> handler_) : handler(handler_) {}
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return handler; }
    virtual CefRefPtr<CefAudioHandler> GetAudioHandler() { return handler; }

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

void PlayerHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    getViewRect(rect);
}

void getViewRect(CefRect& rect) {
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
}

void PlayerHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    const void* buffer, int width, int height)
{
    if (type == PET_VIEW) {
        //버퍼 전달
    }
}

bool PlayerHandler::GetAudioParameters(CefRefPtr<CefBrowser> browser, CefAudioParameters& params) {
    auto ret = CefAudioHandler::GetAudioParameters(browser, params);
    params.sample_rate = sampleRate;
    params.frames_per_buffer = samplesPerBlock;

    return ret;
}

void PlayerHandler::OnAudioStreamStarted(CefRefPtr<CefBrowser> browser, const CefAudioParameters& params, int channel) {
    channels = channel;

    if (qChannels != channel) {
        while (!q.empty()) q.pop();
        qChannels = channel;
    }
}

void PlayerHandler::OnAudioStreamPacket(CefRefPtr<CefBrowser> browser, const float** data, int frames, long long pts) {
    //bit depth: 32
    for (int i = 0; i < channels; i++) {
        this->buffer.push(i, data[i], frames);
    }

    //TODO (mixer)

    //TODO: making buffer using algorithm (maybe queue?)
    //Player Buffer : Accumulated (48khz)
    //Conversion : Mixer (Sample Rate)
    //Output Buffer : whatever you want (using conversion)
}

void PlayerHandler::OnAudioStreamStopped(CefRefPtr<CefBrowser> browser) {
    while (!q.empty()) q.pop();
}

void PlayerHandler::OnAudioStreamError(CefRefPtr<CefBrowser> browser, const CefString& message) {

}

static CefRefPtr<CefApp> app;
static CefRefPtr<CefBrowser> browser;
static CefRefPtr<PlayerBrowserClient> client;
static CefRefPtr<PlayerHandler> handler;

static std::queue<float> q;
static int qChannels = 2;

static double sampleRate = 44100.0;
static int samplesPerBlock = 256;
static int channels = 2;

static int width = 800;
static int height = 600;

extern "C" {
    CEFWRAPPER_API void resized() {
        if (browser) {
            browser->GetHost()->WasResized();
            browser->GetHost()->Invalidate(PET_VIEW);
        }
    }

    CEFWRAPPER_API int initializeCEF(const char* path, wchar_t* cache) {
        CefMainArgs args;
        CefSettings settings;

        settings.windowless_rendering_enabled = true;
        settings.multi_threaded_message_loop = false;
        settings.no_sandbox = true;

        CefString(&settings.browser_subprocess_path) = path;
        CefString(&settings.root_cache_path).FromWString(cache);

        app = new PlayerApp();
        bool init = CefInitialize(args, settings, app.get(), nullptr);
        if (!init) return 0;

        handler = new PlayerHandler();
        client = new PlayerBrowserClient(handler);

        CefWindowInfo info;
        info.windowless_rendering_enabled = true;
        info.SetAsWindowless(nullptr);

        CefBrowserSettings settings2;
        settings2.windowless_frame_rate = 60;
        settings2.background_color = CefColorSetARGB(255, 255, 255, 255);

        auto url = CefString("https://youtube.com/embed/HgzGwKwLmgM"); //Default Video: Queen - Don't Stop Me Now
        browser = CefBrowserHost::CreateBrowserSync(info, client, url, settings2, nullptr, nullptr);

        resized();
        return 1;
    }

    CEFWRAPPER_API void shutdownCEF() {
        if (browser) { //if browser is not nullptr
            browser->GetHost()->CloseBrowser(true);
            browser = nullptr;
        }

        if (CefCurrentlyOn(TID_UI)) {
            CefShutdown();
        }
    }

    CEFWRAPPER_API void loadURL(const char* url) {
        if (browser) {
            browser->GetMainFrame()->LoadURL(CefString(url));
        }
    }

    CEFWRAPPER_API void timerCallback() {
        CefDoMessageLoopWork();
    }

    CEFWRAPPER_API void setLocalBounds(int width_, int height_) {
        width = width_;
        height = height_;
    }

    CEFWRAPPER_API void mouseMove(int x, int y) {
        if (browser) {
            CefMouseEvent event;
            event.x = x;
            event.y = y;

            browser->GetHost()->SendMouseMoveEvent(event, false);
        }
    }

    CEFWRAPPER_API void mouseDown(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount) {
        if (browser) {
            CefMouseEvent event;
            event.x = x;
            event.y = y;
            int type = 0;

            if (isLeft) type = 0;
            else if (isMiddle) type = 1;
            else if (isRight) type = 2;

            browser->GetHost()->SendMouseClickEvent(event, (CefBrowserHost::MouseButtonType)type, false, clickCount);
        }
    }

    CEFWRAPPER_API void mouseUp(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount) {
        if (browser) {
            CefMouseEvent event;
            event.x = x;
            event.y = y;
            int type = 0;

            if (isLeft) type = 0;
            else if (isMiddle) type = 1;
            else if (isRight) type = 2;

            browser->GetHost()->SendMouseClickEvent(event, (CefBrowserHost::MouseButtonType)type, true, clickCount);
        }
    }
}