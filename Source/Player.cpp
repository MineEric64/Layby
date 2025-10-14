/*
  ==============================================================================

    Player.cpp
    Created: 13 Oct 2025 4:19:33am
    Author:  MineEric64

  ==============================================================================
*/

#include "Player.h"

AudioSampleBufferV2 PlayerHandler::buffer(PlayerHandler::channel);

void PlayerHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    player->getViewRect(rect);
}

void PlayerHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    const void* buffer, int width, int height)
{
    if (type == PET_VIEW) {
        player->updateImage(buffer, width, height);
    }
}

Player::Player() {
    setSize(800, 600);
    startTimer(16); //60fps
}

Player::~Player() {
    shutdownCEF();
}

void Player::resized() {
    if (browser) {
        browser->GetHost()->WasResized();
        browser->GetHost()->Invalidate(PET_VIEW);
    }
}

void Player::initializeCEF() {
    CefMainArgs args;
    CefSettings settings;

    settings.windowless_rendering_enabled = true;
    settings.multi_threaded_message_loop = false;
    settings.no_sandbox = true;

    auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    auto subwoofer = executableDir.getChildFile("layby_subwoofer.exe");

    if (!subwoofer.exists()) {
        //ERROR!
    }
    CefString(&settings.browser_subprocess_path) = subwoofer.getFullPathName().toStdString();

    auto cacheDir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("LaybyCEFCache");
    cacheDir.createDirectory();
    CefString(&settings.root_cache_path).FromWString(cacheDir.getFullPathName().toWideCharPointer());

    app = new PlayerApp();
    CefInitialize(args, settings, app.get(), nullptr);
    handler = new PlayerHandler(this);
    client = new PlayerBrowserClient(handler);

    CefWindowInfo info;
    info.windowless_rendering_enabled = true;
    info.SetAsWindowless(nullptr);

    CefBrowserSettings settings2;
    settings2.windowless_frame_rate = 60;
    settings2.background_color = CefColorSetARGB(255, 255, 255, 255);

    url_test = juce::String("https://youtube.com/embed/P-T87k30zZs"); //test
    browser = CefBrowserHost::CreateBrowserSync(info, client, url_test.toStdString(), settings2, nullptr, nullptr);

    resized();
}

void Player::shutdownCEF() {
    if (browser) { //if browser is not nullptr
        browser->GetHost()->CloseBrowser(true);
        browser = nullptr;
    }
}

void Player::loadURL(const juce::String& url) {
    if (browser) {
        browser->GetMainFrame()->LoadURL(url.toStdString());
    }
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

void Player::timerCallback() {
    CefDoMessageLoopWork();
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

void Player::getViewRect(CefRect& rect) {
    auto bounds = getLocalBounds();
    rect.x = 0;
    rect.y = 0;
    rect.width = bounds.getWidth();
    rect.height = bounds.getHeight();
}

void Player::copyBufferToImage(const void* buffer, juce::Image& imageTo) {
    juce::Image::BitmapData bitmap(imageTo, juce::Image::BitmapData::writeOnly); //BGRA
    memcpy(bitmap.data, buffer, image.getWidth() * image.getHeight() * 4);
}

void Player::mouseMove(const juce::MouseEvent& event)
{
    if (browser) {
        CefMouseEvent event2;
        event2.x = event.position.x;
        event2.y = event.position.y;

        browser->GetHost()->SendMouseMoveEvent(event2, false);
    }
}

void Player::mouseDown(const juce::MouseEvent& event) {
    if (browser) {
        CefMouseEvent event2;
        event2.x = event.position.x;
        event2.y = event.position.y;
        int type = 0;

        if (event.mods.isLeftButtonDown()) type = 0;
        else if (event.mods.isMiddleButtonDown()) type = 1;
        else if (event.mods.isRightButtonDown()) type = 2;

        browser->GetHost()->SendMouseClickEvent(event2, (CefBrowserHost::MouseButtonType)type, false, event.getNumberOfClicks());
    }
}

void Player::mouseUp(const juce::MouseEvent& event) {
    if (browser) {
        CefMouseEvent event2;
        event2.x = event.position.x;
        event2.y = event.position.y;
        int type = 0;

        if (event.mods.isLeftButtonDown()) type = 0;
        else if (event.mods.isMiddleButtonDown()) type = 1;
        else if (event.mods.isRightButtonDown()) type = 2;

        browser->GetHost()->SendMouseClickEvent(event2, (CefBrowserHost::MouseButtonType)type, true, event.getNumberOfClicks());
    }
}

bool PlayerHandler::GetAudioParameters(CefRefPtr<CefBrowser> browser, CefAudioParameters& params) {
    auto ret = CefAudioHandler::GetAudioParameters(browser, params);
    params.sample_rate = sampleRate;
    params.frames_per_buffer = samplesPerBlock;

    return ret;
}

void PlayerHandler::OnAudioStreamStarted(CefRefPtr<CefBrowser> browser, const CefAudioParameters& params, int channel) {
    this->channel = channel;

    if (this->buffer.channel != channel) {
        this->buffer.clear();
        this->buffer = AudioSampleBufferV2(channel);
    }
}

void PlayerHandler::OnAudioStreamPacket(CefRefPtr<CefBrowser> browser, const float** data, int frames, long long pts) {
    //bit depth: 32
    for (int i = 0; i < channel; i++) {
        this->buffer.push(i, data[i], frames);
    }

    //TODO (mixer)

    //TODO: making buffer using algorithm (maybe queue?)
    //Player Buffer : Accumulated (48khz)
    //Conversion : Mixer (Sample Rate)
    //Output Buffer : whatever you want (using conversion)
}

void PlayerHandler::OnAudioStreamStopped(CefRefPtr<CefBrowser> browser) {
    this->buffer.clear();
}

void PlayerHandler::OnAudioStreamError(CefRefPtr<CefBrowser> browser, const CefString& message) {

}