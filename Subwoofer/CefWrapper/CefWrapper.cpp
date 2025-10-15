#include <queue>
#include <string>
#include <vector>

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

static CefRefPtr<CefApp> app;
static CefRefPtr<CefBrowser> browser;
static CefRefPtr<PlayerBrowserClient> client;
static CefRefPtr<PlayerHandler> handler;

static std::queue<std::vector<float>> q; //vector: channel
static int qChannels = 2;

static double sampleRate = 44100.0;
static int samplesPerBlock = 256;
static int channels = 2;

static int width = 800;
static int height = 600;

static void* imageBuffer = nullptr;
static int imageWidth;
static int imageHeight;

void PlayerHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    getViewRect(rect);
}

void getViewRect(CefRect& rect) {
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
}

void qClear() {
    while (!q.empty()) {
        auto v = q.front();

        v.clear();
        q.pop();
    }
}

void PlayerHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    const void* buffer, int width, int height)
{
    if (type == PET_VIEW) {
        if (imageBuffer) {
            free(imageBuffer);
            imageBuffer = nullptr;
        }

        int size = width * height * 4;
        imageWidth = width;
        imageHeight = height;

        imageBuffer = malloc(size);
        memcpy(imageBuffer, buffer, size);
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
        qClear();
        qChannels = channel;
    }
}

void PlayerHandler::OnAudioStreamPacket(CefRefPtr<CefBrowser> browser, const float** data, int frames, long long pts) {
    //bit depth: 32
    for (int i = 0; i < frames; i++) {
        std::vector<float> v;
            
        for (int j = 0; j < channels; j++) v.push_back(data[j][i]);
        q.push(v);
    }
}

void PlayerHandler::OnAudioStreamStopped(CefRefPtr<CefBrowser> browser) {
    qClear();
}

void PlayerHandler::OnAudioStreamError(CefRefPtr<CefBrowser> browser, const CefString& message) {

}

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

        qClear();
        free(imageBuffer);
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

    CEFWRAPPER_API void setAudioParam(int sampleRate_, int samplesPerBlock_) {
        sampleRate = sampleRate_;
        samplesPerBlock = samplesPerBlock_;
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

    CEFWRAPPER_API void getImage(void* p, int* width_, int* height_) {
        p = imageBuffer;
        *width_ = imageWidth;
        *height_ = imageHeight;
    }

    CEFWRAPPER_API void getAudioBuffer(float** data, int length) {
        int length2 = min(length, q.size());

        for (int i = 0; i < length2; i++) {
            auto v = q.front();
            q.pop();

            for (int j = 0; j < qChannels; j++) data[j][i] = v[j];
            v.clear();
        }
    }
}