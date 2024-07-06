#include "Reverb.h"

//==============================================================================
Reverb::Reverb()
{
}

Reverb::~Reverb()
{
}

//==============================================================================
void Reverb::allPass(juce::AudioBuffer<float>& dryBuffer, int channel)
{
    auto bufferSize = dryBuffer.getNumSamples();
    float allPassGain = 0.7f;
    float drySample = 0.0f;
    float wetSample = 0.0f;

    //AllPass difference equation: y[n] = g​ain*x[n] + x[n-1] - gain*[n-1]

    for (channel = 0; channel < dryBuffer.getNumChannels(); ++channel)
    {
        for (int sample = 0; sample < dryBuffer.getNumSamples(); ++sample)
        {
            drySample = dryBuffer.getSample(channel, sample);
            wetSample = allPassGain * drySample + oldDrySample - allPassGain * oldWetSample;
            dryBuffer.setSample(channel, sample, wetSample);
        }
    }
}

void Reverb::reverb(juce::AudioBuffer<float>& dryBuffer)
{
    for (int i = 0; i < numAllPass; ++i)
    {
        allPass(dryBuffer, getNumInputChannels());
    }
}

