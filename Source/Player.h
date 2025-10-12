/*
  ==============================================================================

    Player.h
    Created: 13 Oct 2025 4:19:42am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Player : public juce::Component {
public:
    Player();
    void resized() override;

private:
    juce::WebBrowserComponent browser;
};