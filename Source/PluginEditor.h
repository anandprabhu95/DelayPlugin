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

    class DelayAudioProcessorEditor::SliderRange
    {
    public:
        double minVal;
        double maxVal;
        double intervalVal;
        SliderRange(double minimum, double maximum, double interval)
        {
            minVal = minimum;
            maxVal = maximum;
            intervalVal = interval;
        }
    };

    void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g);
    void DelayAudioProcessorEditor::createGUI();
    void DelayAudioProcessorEditor::destroyGUI();
    void DelayAudioProcessorEditor::resizeGUI();
    void DelayAudioProcessorEditor::paramAttacher();
    void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved);

    void DelayAudioProcessorEditor::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
                                                juce::String componentName, DelayAudioProcessorEditor::SliderRange sliderRange);
    void DelayAudioProcessorEditor::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName);
    void DelayAudioProcessorEditor::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, juce::Justification justification);
    void DelayAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider);
    void DelayAudioProcessorEditor::initializeValueLabel(std::unique_ptr<juce::Label>& label);
    void DelayAudioProcessorEditor::setValueLabel(std::unique_ptr<juce::Label>& label, std::unique_ptr<juce::Slider>& slider) const;

    void DelayAudioProcessorEditor::componentDisable();
    void DelayAudioProcessorEditor::hideDelayMsSliderIfBpmSync();

    std::unique_ptr<juce::Slider> m_gainSliderLeft, m_delayMsSliderLeft, m_delayBpmSliderLeft, 
                                  m_gainSliderRight, m_delayMsSliderRight, m_delayBpmSliderRight, 
                                  m_drywetSlider, m_lfoFreqSlider, m_lfoAmtSlider;

    std::unique_ptr<juce::ToggleButton> m_lfoButton, m_testReverbButton, m_stereoDelayButton, m_bpmSyncButtonLeft, m_bpmSyncButtonRight;

    std::unique_ptr<juce::Label> m_gainLabelLeft, m_delayMsLabelLeft, m_gainLabelRight, m_delayMsLabelRight, m_stereoDelayLabel,
                                 m_drywetLabel, m_lfoButtonLabel, m_lfoFreqLabel, m_lfoAmtLabel, m_testRvrbLabel,
                                 m_bpmSyncButtonLabelLeft, m_bpmSyncButtonLabelRight;

    std::unique_ptr<juce::Label> m_gainSliderValDispLeft, m_delayMsSliderValDispLeft, m_delayBpmSliderValDispLeft,
                                 m_gainSliderValDispRight, m_delayMsSliderValDispRight, m_delayBpmSliderValDispRight,
                                 m_drywetSliderValDisp, m_lfoFreqSliderValDisp;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_gainParamAttachLeft, m_delayMsParamAttachLeft, m_delayBpmParamAttachLeft,
                                                                          m_gainParamAttachRight, m_delayMsParamAttachRight, m_delayBpmParamAttachRight,
                                                                          m_drywetParamAttach, m_lfoFreqParamAttach,
                                                                          m_lfoAmtParamAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_lfoButtonParamAttach, m_testRvrbButParamAttach, m_stereoDelayButParamAttach,
                                                                          m_bpmSyncButParamAttachLeft, m_bpmSyncButParamAttachRight;

    //======================================================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
