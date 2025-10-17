/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "Player.h"
#include "IdParser.h"

//==============================================================================
/**
*/
class TestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TestAudioProcessorEditor (TestAudioProcessor&);
    ~TestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    Player player;
    IdParser parser;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestAudioProcessorEditor)
};
