/*
  ==============================================================================

    AudioSampleBufferV2.h
    Created: 14 Oct 2025 10:38:10pm
    Author:  MineEric64

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//Queue
class AudioSampleBufferV2 {
public:
    int channel;
    int capacity;
    int size = 0;

    AudioSampleBufferV2(const int channel);
    AudioSampleBufferV2(const int channel, const int capacity);
    ~AudioSampleBufferV2();

    void push(const int destChannel, const float*& data, const int length);
    void popAndAdd(const int destChannel, const int length, juce::AudioSampleBuffer& source);
    void clear();
        
private:
    juce::Array<juce::AbstractFifo*> q;
    juce::AudioSampleBuffer qbuffer;
};