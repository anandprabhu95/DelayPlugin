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
    void DelayAudioProcessorEditor::paramAttacher();
    void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved);

    juce::ScopedPointer<juce::Slider> gainSlider, delayMsSlider, drywetSlider;
    juce::ScopedPointer<juce::Label> gainLabel, delayMsLabel, drywetLabel;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> gainParamAttach, delayMsParamAttach, drywetParamAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
