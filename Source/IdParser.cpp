/*
  ==============================================================================

    IdParser.cpp
    Created: 17 Oct 2025 2:42:19am
    Author:  MineEric64

  ==============================================================================
*/

#include "IdParser.h"
#include "Player.h"
#define DEBUG(message, title) juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, title, message)

IdParser::IdParser() {
    addAndMakeVisible(textEditor);
    addAndMakeVisible(loadButton);

    textEditor.setTextToShowWhenEmpty("Enter YouTube URL...", juce::Colours::grey);
    textEditor.applyFontToAllText(juce::FontOptions(20.0));

    loadButton.addListener(this);

    setSize(440, 30);
}

IdParser::~IdParser() {
    loadButton.removeListener(this);
}

void IdParser::resized() {
    auto bounds = getLocalBounds();

    loadButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 4));
    textEditor.setBounds(bounds);
}

void IdParser::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::whitesmoke);
}

void IdParser::textEditorTextChanged(juce::TextEditor& editor)
{

}

void IdParser::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    loadButton.triggerClick();
}

void IdParser::textEditorEscapeKeyPressed(juce::TextEditor& editor) {

}

void IdParser::textEditorFocusLost(juce::TextEditor& editor) {

}

void IdParser::buttonClicked(juce::Button* button) {
    juce::String url = parseVideoId(textEditor.getText());

    if (url.length() <= 0) {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Layby", "Please input the valid URL.");
        return;
    }
    if (Player::cef.loadURL != NULL) Player::cef.loadURL(url.toRawUTF8());
}

juce::String IdParser::parseVideoId(juce::String url) {
    juce::String urlOriginal(url);

    //HTTP / HTTPS
    if (url.startsWithIgnoreCase("http://")) url = url.substring(7);
    else if (url.startsWithIgnoreCase("https://")) url = url.substring(8);

    //WWW
    if (url.startsWithIgnoreCase("www.")) url = url.substring(4);
    
    //Domain Name
    juce::String urlBase = juce::String("https://www.youtube.com/embed/");

    if (url.startsWithIgnoreCase("youtube.com")) {
        url = url.substring(11);

        if (url.startsWithIgnoreCase("/watch?")) {
            juce::URL url2(urlOriginal);

            auto names = url2.getParameterNames();
            auto values = url2.getParameterValues();

            for (int i = 0; i < names.size(); i++) {
                if (names[i] == "v" && values[i].length() == 11) {
                    url = values[i];
                    break;
                }
            }

            //id must be 11 characters!
            if (url.length() == 11) urlBase += url;
        }
        else if (url.startsWithIgnoreCase("/embed/")) {
            url = url.substring(7);
            
            if (url.length() >= 11) {
                url = url.substring(0, 11);
                urlBase += url;
            }
        }
        else if (url.startsWithIgnoreCase("/v/")) {
            url = url.substring(3);
            
            if (url.length() >= 11) {
                url = url.substring(0, 11);
                urlBase += url;
            }
        }
        else if (url == "/" || url.length() == 0 || url == "/watch" || url == "/v" || url == "/v/") urlBase = juce::String("https://www.youtube.com"); //it's just youtube main homepage
    }
    else if (url.startsWithIgnoreCase("youtu.be")) {
        url = url.substring(8);

        if (url == "/" || url.length() == 0) urlBase = juce::String("https://www.youtube.com"); //it's just youtube main homepage
        else if (url.startsWithIgnoreCase("/") && url.length() >= 12) {
            url = url.substring(1, 12);
            urlBase += url;
        }
    }

    if (urlBase == "https://www.youtube.com/embed/") return juce::String();
    return urlBase;
}