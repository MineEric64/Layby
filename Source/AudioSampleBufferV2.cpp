/*
  ==============================================================================

    AudioSampleBufferV2.cpp
    Created: 14 Oct 2025 10:38:10pm
    Author:  MineEric64

  ==============================================================================
*/

#include "AudioSampleBufferV2.h"

AudioSampleBufferV2::AudioSampleBufferV2(const int channel, const int capacity) : q(), qbuffer(channel, capacity)
{
    this->channel = channel;
    this->capacity = capacity;

    for (int i = 0; i < channel; i++) q.add(new juce::AbstractFifo(capacity));
}

AudioSampleBufferV2::AudioSampleBufferV2(const int channel) : AudioSampleBufferV2(channel, 48000) {}

AudioSampleBufferV2::~AudioSampleBufferV2() {
    clear();
    for (auto* fifo : q) delete fifo;
    q.clear();
}

void AudioSampleBufferV2::push(const int destChannel, const float*& data, const int length) {
    int start1, start2, size1, size2;

    q[destChannel]->prepareToWrite(length, start1, size1, start2, size2);
    if (size1 > 0) qbuffer.copyFrom(destChannel, start1, data, size1);
    if (size2 > 0) qbuffer.copyFrom(destChannel, start2, data + size1, size2);
    q[destChannel]->finishedWrite(size1 + size2);

    size += size1 + size2;
}

void AudioSampleBufferV2::popAndAdd(const int destChannel, const int length, juce::AudioSampleBuffer& source) {
    int length2 = length <= source.getNumSamples() ? length : source.getNumSamples();
    int start1, start2, size1, size2;

    if (size < length2) return;
    
    q[destChannel]->prepareToRead(length2, start1, size1, start2, size2);
    if (size1 > 0) source.copyFrom(destChannel, 0, qbuffer, destChannel, start1, size1);
    if (size2 > 0) source.copyFrom(destChannel, size1, qbuffer, destChannel, start2, size2);
    q[destChannel]->finishedRead(size1 + size2);

    size -= length2;
}

void AudioSampleBufferV2::clear() {
    for (auto* fifo : q) fifo->reset();
    qbuffer.clear();
}