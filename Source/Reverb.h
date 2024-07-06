#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

class Reverb : public juce::AudioProcessor
{
public:
    Reverb();
    ~Reverb();

    //==============================================================================
    void Reverb::reverb(juce::AudioBuffer<float>& dryBuffer);
    int numAllPass = 4;

private:
    void Reverb::allPass(juce::AudioBuffer<float>& dryBuffer, int channel);

    float oldDrySample = 0.0f;
    float oldWetSample = 0.0f;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};
