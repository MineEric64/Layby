/*
  ==============================================================================

    IdParser.h
    Created: 17 Oct 2025 2:42:19am
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class IdParser : public juce::Component, public juce::TextEditor::Listener, public juce::Button::Listener
{
public:
    IdParser();
    ~IdParser();

    void resized() override;
    void paint(juce::Graphics& g) override;
    void textEditorTextChanged(juce::TextEditor& editor) override;
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& editor) override;
    void textEditorFocusLost(juce::TextEditor& editor) override;

    juce::String parseVideoId(juce::String url, bool& isHome);

private:
    juce::TextEditor textEditor;
    juce::TextButton loadButton { "Enter" };
    //juce::TextButton homeButton { "Home" };

    void buttonClicked(juce::Button* button) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IdParser)
};