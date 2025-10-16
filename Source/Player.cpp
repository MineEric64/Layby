/*
  ==============================================================================

    Player.cpp
    Created: 13 Oct 2025 4:19:33am
    Author:  MineEric64

  ==============================================================================
*/

#include "Player.h"

Player::Player() {
    setSize(800, 600);
    startTimer(16); //60fps
}

Player::~Player() {
    if (cef.shutdownCEF != NULL) cef.shutdownCEF();
}

void Player::initializeCEF() {
    auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    auto subwoofer = executableDir.getChildFile("layby_subwoofer.exe");

    if (!subwoofer.exists()) {
        //ERROR!
    }

    auto cacheDir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("LaybyCEFCache");
    cacheDir.createDirectory();
    auto* cache = cacheDir.getFullPathName().toWideCharPointer();

    if (cef.initializeCEF != NULL) cef.initializeCEF(subwoofer.getFullPathName().toRawUTF8(), cache);
}

void Player::paint(juce::Graphics& g)
{
    if (image.isValid()) {
        g.drawImage(image, getLocalBounds().toFloat());
    }
    else {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText("Waiting for CEF...", getLocalBounds(), juce::Justification::centred);
    }
}

void Player::resized() {
    if (cef.resized != NULL) cef.resized();
}

void Player::timerCallback() {
    if (cef.timerCallback != NULL) cef.timerCallback();
}

void Player::updateImage(const void* buffer, int width, int height) {
    juce::ScopedLock lock(imageMutex);

    if (image.getWidth() != width || image.getHeight() != height)
        image = juce::Image(juce::Image::PixelFormat::ARGB, width, height, true);

    if (buffer && image.isValid()) {
        copyBufferToImage(buffer, image);
    }

    juce::MessageManager::callAsync([this] { repaint(); });
}

void Player::copyBufferToImage(const void* buffer, juce::Image& imageTo) {
    juce::Image::BitmapData bitmap(imageTo, juce::Image::BitmapData::writeOnly); //BGRA
    memcpy(bitmap.data, buffer, image.getWidth() * image.getHeight() * 4);
}

void Player::mouseMove(const juce::MouseEvent& event)
{
    if (cef.mouseMove != NULL) cef.mouseMove(event.position.x, event.position.y);
}

void Player::mouseDown(const juce::MouseEvent& event) {
    if (cef.mouseDown != NULL) cef.mouseDown(event.position.x, event.position.y, event.mods.isLeftButtonDown(), event.mods.isMiddleButtonDown(), event.mods.isRightButtonDown(), event.getNumberOfClicks());
}

void Player::mouseUp(const juce::MouseEvent& event) {
    if (cef.mouseUp != NULL) cef.mouseUp(event.position.x, event.position.y, event.mods.isLeftButtonDown(), event.mods.isMiddleButtonDown(), event.mods.isRightButtonDown(), event.getNumberOfClicks());
}