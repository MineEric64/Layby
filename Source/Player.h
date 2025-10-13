/*
  ==============================================================================

    Player.h
    Created: 13 Oct 2025 6:19:35am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")
#include <JuceHeader.h>
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/wrapper/cef_helpers.h"

class PlayerHandler : public CefRenderHandler, public CefAudioHandler {
public:
    //Video
    PlayerHandler(class Player* player_) : player(player_) {}
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

    static juce::AudioSampleBuffer buffer;
    static inline double sampleRate = 44100.0;
    static inline int samplesPerBlock = 256;
    static inline int channel = 2;

private:
    Player* player;
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

class Player : public juce::Component, public juce::Timer {
public:
    Player();
    ~Player();

    void resized() override;
    void paint(juce::Graphics& g) override;
    void timerCallback() override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    juce::String url_test;

    //CEF
    void initializeCEF();
    void shutdownCEF();
    void loadURL(const juce::String& url);

    //from PlayerRenderHandler 
    void updateImage(const void* buffer, int width, int height);
    void getViewRect(CefRect& rect);

private:
    CefRefPtr<CefApp> app;
    CefRefPtr<CefBrowser> browser;
    CefRefPtr<PlayerBrowserClient> client;
    CefRefPtr<PlayerHandler> handler;

    juce::Image image;
    juce::CriticalSection imageMutex;

    void copyBufferToImage(const void* buffer, juce::Image& imageTo);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};