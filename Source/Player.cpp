/*
  ==============================================================================

    Player.cpp
    Created: 13 Oct 2025 4:19:33am
    Author:  MineEric64

  ==============================================================================
*/

#define WIDTH 800
#define HEIGHT 540
#include "Player.h"
#include "IdParser.h"
#include "TimeShop.h"

CefLoader Player::cef;

Player::Player() : delayRemain(0) {
    setSize(WIDTH, HEIGHT);
    startTimer(16); //60fps

    setWantsKeyboardFocus(true);
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

    bypassed = false;

    TimeShop::callAfterDelay(bypass, 1000);
    TimeShop::callAfterDelay(bypass, 2000); //for safe
}

juce::String Player::getURL() {
    if (cef.getURL != NULL) {
        char url[255];
        
        cef.getURL(url);
        return juce::String(url);
    }
}

void Player::loadURL(const juce::String& url) {
    if (Player::cef.loadURL != NULL) Player::cef.loadURL(url.toRawUTF8());
    bypassed = false;

    TimeShop::callAfterDelay(bypass, 500);
    TimeShop::callAfterDelay(bypass, 1000); //for safe
}

//Middle Mouse Click Event (for autoplay, YouTube Iframe API)
void Player::bypass() {
    mouseDownInternal(WIDTH / 2, HEIGHT / 2, 0, 1, 0, 1);
    juce::Thread::sleep(30);
    mouseUpInternal(WIDTH / 2, HEIGHT / 2, 0, 1, 0, 1);
    bypassed = true;
}

void Player::setupCallback(std::function<void()> f, int delay) {
    callback = std::move(f);
    delayRemain = delay;
}

void Player::paint(juce::Graphics& g)
{
    if (image.isValid()) {
        juce::RectanglePlacement placement(juce::RectanglePlacement::xMid | juce::RectanglePlacement::yTop);
        g.drawImage(image, getLocalBounds().toFloat(), placement);
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

    //Callback
    if (delayRemain > 0 && delayRemain <= 16 && callback) {
        if (callback) callback();
        delayRemain = 0;
    }
    else if (delayRemain > 16) delayRemain -= 16;
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
    mouseDownInternal(event.position.x, event.position.y, event.mods.isLeftButtonDown(), event.mods.isMiddleButtonDown(), event.mods.isRightButtonDown(), event.getNumberOfClicks());
}

void Player::mouseUp(const juce::MouseEvent& event) {
    mouseUpInternal(event.position.x, event.position.y, event.mods.isLeftButtonDown(), event.mods.isMiddleButtonDown(), event.mods.isRightButtonDown(), event.getNumberOfClicks());
}

void Player::mouseDownInternal(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount) {
    if (cef.mouseDown != NULL) cef.mouseDown(x, y, isLeft, isMiddle, isRight, clickCount);
}

void Player::mouseUpInternal(int x, int y, int isLeft, int isMiddle, int isRight, int clickCount) {
    if (cef.mouseUp != NULL) cef.mouseUp(x, y, isLeft, isMiddle, isRight, clickCount);
}

bool Player::keyPressed(const juce::KeyPress& key)
{
    auto ch = key.getTextCharacter();
    bool result = false;

    //unsigned int to char16_t
    char16_t ch2 = u'\0';
    if (ch != 0 && ch <= 0xFFFF) ch2 = static_cast<char16_t>(ch);

    // If return's value is false, allow other components to handle the event
    if (cef.keyPressed != NULL && ch2 != 0) result = cef.keyPressed(ch2, key.getKeyCode());
    return result;
}

void Player::playVideo() {
    if (cef.executeJS != NULL) cef.executeJS("window.postMessage('{\"event\":\"command\",\"func\":\"playVideo\"}', 'https://www.youtube.com');");
}

void Player::pauseVideo() {
    if (cef.executeJS != NULL) cef.executeJS("window.postMessage('{\"event\":\"command\",\"func\":\"pauseVideo\"}', 'https://www.youtube.com');");
}

void Player::stopVideo() {
    if (cef.executeJS != NULL) cef.executeJS("window.postMessage('{\"event\":\"command\",\"func\":\"stopVideo\"}', 'https://www.youtube.com');");
}

void Player::seekTo(float time) {
    if (cef.executeJS != NULL) {
        juce::String script("window.postMessage('{\"event\":\"command\",\"func\":\"seekTo\", \"args\":[");
       
        script += juce::String(time);
        script += juce::String(", true]}', 'https://www.youtube.com');");
        cef.executeJS(script.toRawUTF8());
    }
}