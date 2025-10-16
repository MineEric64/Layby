/*
  ==============================================================================

    Player.cpp
    Created: 13 Oct 2025 4:19:33am
    Author:  MineEric64

  ==============================================================================
*/

#define WIDTH 800
#define HEIGHT 600
#include "Player.h"

CefLoader Player::cef;

Player::Player() {
    setSize(WIDTH, HEIGHT);
    startTimer(16); //60fps
}

Player::~Player() {
    stopTimer();
}

void Player::initializeCEF() {
    if (cef.handle == NULL) return;
    if (cefInit || (cef.isInitialized != NULL && cef.isInitialized())) return;

    auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    auto subwoofer = executableDir.getChildFile("layby_subwoofer.exe");

    if (!subwoofer.exists()) {
        //ERROR!
    }

    auto cacheDir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("LaybyCEFCache");
    cacheDir.createDirectory();
    auto* cache = cacheDir.getFullPathName().toWideCharPointer();

    if (cef.setLocalBounds != NULL) cef.setLocalBounds(WIDTH, HEIGHT);
    if (cef.initializeCEF != NULL) cefInit = cef.initializeCEF(subwoofer.getFullPathName().toRawUTF8(), cache);
}

void Player::paint(juce::Graphics& g)
{
    if (image.isValid()) {
        g.drawImage(image, getLocalBounds().toFloat(), juce::RectanglePlacement::doNotResize);
    }
    else {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.drawText("Waiting for CEF...", getLocalBounds(), juce::Justification::centred);
    }
}

void Player::resized() {
    if (cef.setLocalBounds != NULL) {
        auto bounds = getLocalBounds();
        cef.setLocalBounds(bounds.getWidth(), bounds.getHeight());
    }
    if (cef.resized != NULL) cef.resized();
}

void Player::timerCallback() {
    if (cef.timerCallback != NULL) cef.timerCallback();
    if (cef.getImageSize != NULL && cef.getImage != NULL) {
        int width, height;
        cef.getImageSize(&width, &height);

        if (width > 0 && height > 0) {
            int length = width * height * 4;
            void* buffer = malloc(length);
            int success = cef.getImage(buffer, length);

            if (success) updateImage(buffer, width, height);
            free(buffer);
        }
    }
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