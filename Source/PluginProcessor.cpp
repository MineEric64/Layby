/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestAudioProcessor::TestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TestAudioProcessor::~TestAudioProcessor()
{
    if (Player::cef.shutdownCEF != NULL) Player::cef.shutdownCEF();
    //Player::cef.kill();
    Player::cef.free();

    Player::cefInit = false;
    Player::cef = CefLoader();
}

//==============================================================================
const juce::String TestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TestAudioProcessor::getProgramName (int index)
{
    return {};
}

void TestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if (!Player::cefInit) {
        //TODO: change to Resources path
        auto executableDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
        auto executableDirPath = executableDir.getFullPathName();
        executableDirPath.append("\\", 1);
        auto dll = executableDir.getChildFile("CefWrapper.dll").getFullPathName();

        Player::cef.init(executableDirPath.toWideCharPointer(), dll.toWideCharPointer());
        Player::initializeCEF();
    }

    if (Player::cef.setAudioParam != NULL) Player::cef.setAudioParam((int)sampleRate, samplesPerBlock);
}

void TestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    int channels = buffer.getNumChannels();

    //Clear the buffer
    buffer.clear();

    //Get audio buffer
    float** buffer2 = (float**)malloc(sizeof(float*) * channels);
    if (buffer2 != NULL) for (int i = 0; i < channels; i++) buffer2[i] = (float*)calloc(buffer.getNumSamples(), 4);
    int length = -1;
    
    if (buffer2 != NULL && Player::cef.getAudioBuffer != NULL) length = Player::cef.getAudioBuffer(buffer2, buffer.getNumSamples(), channels);
    
    for (int i = 0; i < channels; i++) {
        if (length > 0) { //if below zero, There's no audio data to copy

            //TODO: Mixer
            //https://forum.juce.com/t/sample-rate-conversion-between-44-1-khz-and-48-khz/38147/6
            //https://github.com/libsndfile/libsamplerate

            buffer.copyFrom(i, 0, buffer2[i], length);
        }
        if (buffer2 != NULL && buffer2[i] != NULL) free(buffer2[i]);
    }

    if (buffer2 != NULL) free(buffer2);
}

//==============================================================================
bool TestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TestAudioProcessor::createEditor()
{
    auto* editor = new TestAudioProcessorEditor(*this);
    return editor;
}

//==============================================================================
void TestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TestAudioProcessor();
}
