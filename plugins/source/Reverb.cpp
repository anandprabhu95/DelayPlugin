#include "Delay/Reverb.h"

//==============================================================================
Reverb::Reverb()
{
    revBuffer.setSize(2, 96000);
}

Reverb::~Reverb()
{
}

//==============================================================================
void Reverb::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto revBufferSize = revBuffer.getNumSamples();

    // Check if main buffer can be copied to delay buffer without wrapping around
    if (revBufferSize >= bufferSize + writePosition)
    {
        // Copy main buffer to delay buffer.
        revBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), bufferSize);
    }
    else
    {
        // Check how much space is left in the delay buffer.
        auto numSamplesToEnd = revBufferSize - writePosition;
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy the samples to the end
        revBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), numSamplesToEnd);

        // Copy the rest from the start position of the buffer.
        revBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void Reverb::readFromBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto revBufferSize = revBuffer.getNumSamples();

    //auto* delayTimePointer = params.getRawParameterValue("DELAYMS");
    //delayTimeInterpolator.setTargetValue(delayTimePointer->load());
    //float delayTime = delayTimeInterpolator.getNextValue();
    float delayTime = 100; // Samples

    //auto* feedbackGainPointer = params.getRawParameterValue("FEEDBACKGAIN");
    //feedbackGainInterpolator.setTargetValue(feedbackGainPointer->load());
    //float feedbackGain = feedbackGainInterpolator.getNextValue();

    // Read "delayTime" seconds of audio in the past from the delay buffer.
    readPosition = writePosition - static_cast<int>(delayTime);
    auto readPosTemp = readPosition;
    int pos = 0;
    for (int k = 0; k < 40; ++k)
    {
        float gain = feedbackRampDown(k, 40, 1, 0.2);
        DBG("k: " << k);
        // Wrap around.
        if (readPosTemp < 0)
        {
            readPosTemp = revBufferSize + readPosTemp;
        }
        DBG("ReadPsTemp: " << readPosTemp);
        if (readPosTemp + bufferSize < revBufferSize)
        {
            DBG("Trig if");
            buffer.addFromWithRamp(channel, 0, revBuffer.getWritePointer(channel, readPosTemp), bufferSize, gain, gain);
        }
        else
        {
            DBG("Trig else");
            auto numSamplesToEnd = revBufferSize - readPosTemp;
            buffer.addFromWithRamp(channel, 0, revBuffer.getWritePointer(channel, readPosTemp), numSamplesToEnd, gain, gain);
            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            buffer.addFromWithRamp(channel, numSamplesToEnd, revBuffer.getWritePointer(channel, 0), numSamplesAtStart, gain, gain);
        }
        readPosTemp = readPosition - k * static_cast<int>(delayTime);
    }
}

void Reverb::updateWritePositions(juce::AudioBuffer<float>& buffer)
{
    // Loop the write position from 0 to delay buffer size.
    auto bufferSize = buffer.getNumSamples();
    auto revBufferSize = revBuffer.getNumSamples();
    writePosition += bufferSize;
    writePosition %= revBufferSize;
}

void Reverb::reverb(juce::AudioBuffer<float>& buffer, int channel)
{
    for (int j = 0; j < channel; ++j)
    {
        fillBuffer(buffer, j);
        readFromBuffer(buffer, j);
    }
    updateWritePositions(buffer);
}

float Reverb::feedbackRampDown(int index, int indexMax, float startVal, float endVal)
{
    float val = (endVal - startVal) / indexMax * index + startVal;
    return val;
}

