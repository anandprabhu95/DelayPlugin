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


    std::unique_ptr<juce::Slider> m_gainSlider, m_delayMsSlider, m_gainSlider2, m_delayMsSlider2, m_drywetSlider, m_lfoFreqSlider,
                                  m_lfoAmtSlider;
    std::unique_ptr<juce::ToggleButton> m_lfoButton, m_testReverbButton, m_stereoDelayButton;

    std::unique_ptr<juce::Label> m_gainLabel, m_delayMsLabel, m_gainLabel2, m_delayMsLabel2, m_stereoDelayLabel,
                                 m_drywetLabel, m_lfoButtonLabel, m_lfoFreqLabel, m_lfoAmtLabel, m_testRvrbLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_gainParamAttach, m_delayMsParamAttach,
                                                                          m_gainParamAttach2, m_delayMsParamAttach2,
                                                                          m_drywetParamAttach, m_lfoFreqParamAttach,
                                                                          m_lfoAmtParamAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_lfoButtonParamAttach, m_testRvrbButParamAttach, m_stereoDelayButParamAttach;

    //======================================================================================================

    void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
