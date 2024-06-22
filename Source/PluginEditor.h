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

    std::unique_ptr<juce::Slider> gainSlider, delayMsSlider, drywetSlider;
    std::unique_ptr<juce::ToggleButton> lfoButton;

    std::unique_ptr<juce::Label> gainLabel, delayMsLabel, drywetLabel, lfoLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainParamAttach, delayMsParamAttach, drywetParamAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoParamAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
