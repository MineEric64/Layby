/*
  ==============================================================================

    Player.h
    Created: 13 Oct 2025 6:19:35am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#define DEBUG(message, title) juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, title, message)
#include "CefLoader.h"

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
    bool keyPressed(const juce::KeyPress& key) override;

    static void initializeCEF();
    static juce::String getURL();
    static void loadURL(const juce::String& url);
    static void bypass();
    void updateImage(const void* buffer, int width, int height);
    void stopCEFTimer() { stopTimer(); }
    void setupCallback(std::function<void()> f, int delay);

    static void playVideo();
    static void pauseVideo();
    static void stopVideo();
    static void seekTo(float time);

    static CefLoader cef;
    inline static bool cefInit = false;
    inline static bool bypassed = false;

private:
    juce::Image image;
    juce::CriticalSection imageMutex;
    
    std::function<void()> callback;
    int delayRemain;

    void copyBufferToImage(const void* buffer, juce::Image& imageTo);
    static void mouseDownInternal(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);
    static void mouseUpInternal(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};