/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestAudioProcessorEditor::TestAudioProcessorEditor (TestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(player);
    setSize(800, 600);

    //TODO: change to Resources path
    auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    auto executableDirPath = executableDir.getFullPathName();
    executableDirPath.append("\\", 1);
    auto dll = executableDir.getChildFile("CefWrapper.dll").getFullPathName();

    player.cef.init(executableDirPath.toWideCharPointer(), dll.toWideCharPointer());
    player.initializeCEF();
}

TestAudioProcessorEditor::~TestAudioProcessorEditor()
{
}

//==============================================================================
void TestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Welcome!", getLocalBounds(), juce::Justification::centred, 1);
}

void TestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    player.setBounds(getLocalBounds());
}