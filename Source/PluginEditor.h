#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Constants.h"

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
    void DelayAudioProcessorEditor::componentDisable();
    void DelayAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider);


    std::unique_ptr<juce::Slider> gainSlider, delayMsSlider, gainSlider2, delayMsSlider2, drywetSlider, lfoFreqSlider,
                                  lfoAmtSlider;
    std::unique_ptr<juce::ToggleButton> lfoButton, testReverbButton, stereoDelayButton;

    std::unique_ptr<juce::Label> gainLabel, delayMsLabel, gainLabel2, delayMsLabel2, stereoDelayLabel,
                                 drywetLabel, lfoButtonLabel, lfoFreqLabel, lfoAmtLabel, testRvrbLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainParamAttach, delayMsParamAttach,
                                                                          gainParamAttach2, delayMsParamAttach2,
                                                                          drywetParamAttach, lfoFreqParamAttach,
                                                                          lfoAmtParamAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lfoButtonParamAttach, testRvrbButParamAttach, stereoDelayButParamAttach;

    //======================================================================================================

    void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
