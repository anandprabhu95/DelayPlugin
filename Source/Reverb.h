#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

class Reverb
{
public:
    Reverb();
    ~Reverb();

    //==============================================================================
    void Reverb::reverb(juce::AudioBuffer<float>& buffer, int channel);

private:
    void Reverb::fillBuffer(juce::AudioBuffer<float>& buffer, int channel);
    void Reverb::readFromBuffer(juce::AudioBuffer<float>& buffer, int channel);
    void Reverb::updateWritePositions(juce::AudioBuffer<float>& buffer);
    float Reverb::feedbackRampDown(int index, int indexMax, float startVal, float endVal);

    float oldDrySample = 0.0f;
    float oldWetSample = 0.0f;

    int writePosition{ 0 };
    int readPosition{ 0 };

    juce::AudioBuffer<float> revBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};
