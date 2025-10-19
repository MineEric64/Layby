/*
  ==============================================================================

    TimeShop.cpp
    Created: 19 Oct 2025 8:01:47am
    Author:  MineEric64

  ==============================================================================
*/

#include "TimeShop.h"

void TimeShop::callAfterDelay(int milliseconds)
{
    startTimer(milliseconds);
}

void TimeShop::callAfterDelay(std::function<void()> f, int milliseconds) {
    std::thread([=] {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        f();
    }).detach();
}

void TimeShop::timerCallback()
{
    stopTimer();
    if (callback) callback();
}