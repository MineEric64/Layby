/*
  ==============================================================================

    Player.cpp
    Created: 13 Oct 2025 4:19:33am
    Author:  MineEric64

  ==============================================================================
*/

#include "Player.h"

Player::Player() : browser(juce::WebBrowserComponent::Options{}.withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
    .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
    .withUserDataFolder(juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory))))
{
    addAndMakeVisible(browser);

    //test
    browser.goToURL("https://www.youtube.com/embed/mFDToQP-Ecs");
}

void Player::resized() {
    browser.setBounds(getLocalBounds());
}