/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                   public juce::Slider::Listener 
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayAudioProcessor& audioProcessor;

    void DelayAudioProcessorEditor::createGUI();
    void DelayAudioProcessorEditor::destroyGUI();
    void DelayAudioProcessorEditor::resizeGUI();
    void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved);

    juce::ScopedPointer<juce::Slider> gainSlider, delayMsSlider;
    juce::ScopedPointer<juce::Label> gainLabel, delayMsLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> gainParamAttach, delayMsParamAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
