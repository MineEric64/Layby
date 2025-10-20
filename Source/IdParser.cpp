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
    //addAndMakeVisible(homeButton);

    textEditor.setTextToShowWhenEmpty("Enter YouTube URL...", juce::Colours::grey);
    textEditor.applyFontToAllText(juce::FontOptions(20.0));
    textEditor.addListener(this);

    loadButton.addListener(this);
    //homeButton.addListener(this);

    setSize(440, 30);
}

IdParser::~IdParser() {
    loadButton.removeListener(this);
}

void IdParser::resized() {
    auto bounds = getLocalBounds();

    //homeButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 8));
    loadButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 4.3));
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
    //if (editor == textEditor) {}
    loadButton.triggerClick();
    editor.unfocusAllComponents();
}

void IdParser::textEditorEscapeKeyPressed(juce::TextEditor& editor) {

}

void IdParser::textEditorFocusLost(juce::TextEditor& editor) {

}

void IdParser::buttonClicked(juce::Button* button) {
    if (button == &loadButton) {
        bool isHome = false;
        juce::String text = textEditor.getText();
        juce::String id = parseVideoId(text, isHome);
        juce::String url("https://www.youtube.com/embed/");

        if (id.length() > 0) {
            url += id;
            url += juce::String("?enablejsapi=1"); //for YouTube Iframe API

            juce::String simply("https://youtu.be/");
            simply += id;
            textEditor.setText(simply);
        }
        else if (isHome) url = juce::String("https://www.youtube.com");
        else { //it means it's not from youtube website. etc (ex: soundcloud, spotify ...)
            url = text;

            if (url.isEmpty()) {
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, "Layby", "Please input the valid URL.");
                return;
            }
        }
        Player::loadURL(url);
    }
    /*else if (button == &homeButton) {
        juce::String url("https://www.youtube.com");
        if (Player::cef.loadURL != NULL) Player::cef.loadURL(url.toRawUTF8());
    }*/
}

juce::String IdParser::parseVideoId(juce::String url, bool& isHome) {
    juce::String urlOriginal(url);

    //HTTP / HTTPS
    if (url.startsWithIgnoreCase("http://")) url = url.substring(7);
    else if (url.startsWithIgnoreCase("https://")) url = url.substring(8);

    //WWW
    if (url.startsWithIgnoreCase("www.")) url = url.substring(4);
    
    //Domain Name
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
            if (url.length() == 11) return url;
        }
        else if (url.startsWithIgnoreCase("/embed/")) {
            url = url.substring(7);

            if (url.length() >= 11) {
                url = url.substring(0, 11);
                return url;
            }
        }
        else if (url.startsWithIgnoreCase("/v/")) {
            url = url.substring(3);

            if (url.length() >= 11) {
                url = url.substring(0, 11);
                return url;
            }
        }
        else if (url.startsWithIgnoreCase("/live/")) {
            url = url.substring(6);

            if (url.length() >= 11) {
                url = url.substring(0, 11);
                return url;
            }
        }
        else if (url == "/" || url.length() == 0 || url == "/watch" || url == "/v" || url == "/v/") isHome = 1; //it's just youtube main homepage
    }
    else if (url.startsWithIgnoreCase("youtu.be")) {
        url = url.substring(8);

        if (url == "/" || url.length() == 0) isHome = 1; //it's just youtube main homepage
        else if (url.startsWithIgnoreCase("/") && url.length() >= 12) {
            url = url.substring(1, 12);
            return url;
        }
    }

    return juce::String();
}