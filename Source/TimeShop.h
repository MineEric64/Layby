/*
  ==============================================================================

    TimeShop.h
    Created: 19 Oct 2025 8:01:47am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include <JuceHeader.h>

class TimeShop : public juce::Timer
{
public:
    TimeShop(std::function<void()> f) : callback(std::move(f)) {}
    static void callAfterDelay(std::function<void()> f, int milliseconds);
    void callAfterDelay(int milliseconds);

private:
    std::function<void()> callback;
    void timerCallback() override;
};