/*
  ==============================================================================

    Reverb.h
    Created: 6 Jul 2024 5:10:25pm
    Author:  Anand Prabhu

  ==============================================================================
*/

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

private:


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};
