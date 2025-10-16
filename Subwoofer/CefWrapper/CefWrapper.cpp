#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "CefWrapper.h"

static bool init = false;
static CefRefPtr<CefApp> app;
static CefRefPtr<CefBrowser> browser;
static CefRefPtr<PlayerBrowserClient> client;
static CefRefPtr<PlayerHandler> handler;

static std::queue<std::vector<float>> q; //vector: channel
static int qChannels = 2;
static std::mutex qMutex;

static int sampleRate = 48000;
static int samplesPerBlock = 256;
static int channels = 2;

static int width = 800;
static int height = 600;

static void* imageBuffer = nullptr;
static int imageWidth;
static int imageHeight;

void getViewRect(CefRect& rect) {
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
}

unsigned int random(int max) {
    static unsigned int seed = 4937;
    seed = 8253729 * seed + 2396403;
    return seed % (max + 1);
}

void qClear() {
    std::lock_guard<std::mutex> lock(qMutex);
    while (!q.empty()) q.pop();
}

void PlayerHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    getViewRect(rect);
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
        if (imageBuffer) memcpy(imageBuffer, buffer, size);
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
    std::lock_guard<std::mutex> lock(qMutex);

    for (int i = 0; i < frames; i++) {
        std::vector<float> v;
            
        for (int j = 0; j < qChannels; j++) v.push_back(data[j][i]);
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

    CEFWRAPPER_API int initializeCEF(const char* path, const wchar_t* cache) {
        CefMainArgs args;
        CefSettings settings;

        settings.windowless_rendering_enabled = true;
        settings.multi_threaded_message_loop = false;
        settings.no_sandbox = true;

        CefString(&settings.browser_subprocess_path) = path;
        CefString(&settings.root_cache_path).FromWString(cache);

        app = new PlayerApp();
        int init = CefInitialize(args, settings, app.get(), nullptr);
        if (!init) return 0;

        handler = new PlayerHandler();
        client = new PlayerBrowserClient(handler);

        CefWindowInfo info;
        info.windowless_rendering_enabled = true;
        info.SetAsWindowless(nullptr);

        CefBrowserSettings settings2;
        settings2.windowless_frame_rate = 60;
        settings2.background_color = CefColorSetARGB(255, 255, 255, 255);

        auto url = CefString("https://youtube.com/embed/PZz1Gxdb_tA"); //Default Video: AJR - Overture
        if (random(100) == 49) url = CefString("https://youtube.com/embed/dQw4w9WgXcQ"); //...Or, is it?

        browser = CefBrowserHost::CreateBrowserSync(info, client, url, settings2, nullptr, nullptr);

        resized();
        init = true;
        return 1;
    }

    CEFWRAPPER_API void shutdownCEF() {
        if (browser) { //if browser is not nullptr
            browser->GetHost()->CloseBrowser(true);
        }

        if (init && CefCurrentlyOn(TID_UI)) {
            CefShutdown();
        }

        browser = nullptr;
        client = nullptr;
        handler = nullptr;
        app = nullptr;

        qClear();
        if (imageBuffer) free(imageBuffer);

        init = false;
    }

    CEFWRAPPER_API int isInitialized() {
        return init;
    }

    CEFWRAPPER_API void loadURL(const char* url) {
        if (browser) {
            browser->GetMainFrame()->LoadURL(CefString(url));
        }
    }

    CEFWRAPPER_API void timerCallback() {
        if (init) CefDoMessageLoopWork();
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

    CEFWRAPPER_API int getImage(void** p, int* width_, int* height_) {
        if (imageBuffer) {
            *p = imageBuffer;
            *width_ = imageWidth;
            *height_ = imageHeight;
            return 1;
        }
        return 0;
    }

    CEFWRAPPER_API int getAudioBuffer(float** data, int length, int destChannels) {
        std::lock_guard<std::mutex> lock(qMutex);
        int length2 = length <= q.size() ? length : q.size();
        int channels = qChannels <= destChannels ? qChannels : destChannels;
        
        if (length2 == 0) return 0;

        for (int i = 0; i < length2; i++) {
            const auto& v = q.front();
            int channels2 = channels < v.size() ? channels : v.size();

            for (int j = 0; j < channels2; j++) data[j][i] = v[j];
            q.pop();
        }
        return length2;
    }
}