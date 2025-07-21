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
    void DelayAudioProcessorEditor::modifyDelaySliderForBpmSync();


    std::unique_ptr<juce::Slider> m_gainSliderLeft, m_delayMsSliderLeft, m_delayBpmSliderLeft, 
                                  m_gainSliderRight, m_delayMsSliderRight, m_delayBpmSliderRight, 
                                  m_drywetSlider, m_lfoFreqSlider, m_lfoAmtSlider;

    std::unique_ptr<juce::ToggleButton> m_lfoButton, m_testReverbButton, m_stereoDelayButton, m_bpmSyncButtonLeft, m_bpmSyncButtonRight;

    std::unique_ptr<juce::Label> m_gainLabelLeft, m_delayMsLabelLeft, m_gainLabelRight, m_delayMsLabelRight, m_stereoDelayLabel,
                                 m_drywetLabel, m_lfoButtonLabel, m_lfoFreqLabel, m_lfoAmtLabel, m_testRvrbLabel,
                                 m_bpmSyncButtonLabelLeft, m_bpmSyncButtonLabelRight;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_gainParamAttachLeft, m_delayMsParamAttachLeft, m_delayBpmParamAttachLeft,
                                                                          m_gainParamAttachRight, m_delayMsParamAttachRight, m_delayBpmParamAttachRight,
                                                                          m_drywetParamAttach, m_lfoFreqParamAttach,
                                                                          m_lfoAmtParamAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_lfoButtonParamAttach, m_testRvrbButParamAttach, m_stereoDelayButParamAttach,
                                                                          m_bpmSyncButParamAttachLeft, m_bpmSyncButParamAttachRight;

    //======================================================================================================

    void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
