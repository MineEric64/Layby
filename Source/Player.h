/*
  ==============================================================================

    Player.h
    Created: 13 Oct 2025 6:19:35am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
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

    static void initializeCEF();
    void updateImage(const void* buffer, int width, int height);

    static CefLoader cef;
    inline static bool cefInit = false;

private:
    juce::Image image;
    juce::CriticalSection imageMutex;

    void copyBufferToImage(const void* buffer, juce::Image& imageTo);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};