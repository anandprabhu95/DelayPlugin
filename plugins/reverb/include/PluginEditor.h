#pragma once

#include "Headers.h"
#include "Constants.h"
#include "PluginProcessor.h"

//==============================================================================
class ReverbAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                    public juce::Slider::Listener
{
public:
    ReverbAudioProcessorEditor (ReverbAudioProcessor&);
    ~ReverbAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void ReverbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) override;
    void ReverbAudioProcessorEditor::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
                                                    juce::String componentName, juce::String propertyID);
    void ReverbAudioProcessorEditor::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName);
    void ReverbAudioProcessorEditor::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, juce::Justification justification);
    void ReverbAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider);
    void ReverbAudioProcessorEditor::paramAttacher();

    ReverbAudioProcessor& audioProcessor;

    std::unique_ptr<juce::Slider> m_drywetSlider;
    std::unique_ptr<juce::Slider> m_spaceSlider;
    std::unique_ptr<juce::Slider> m_decaySlider;

    std::unique_ptr<juce::Label> m_drywetLabel;
    std::unique_ptr<juce::Label> m_spaceLabel;
    std::unique_ptr<juce::Label> m_decayLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_drywetParamAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_spaceParamAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_decayParamAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessorEditor)
};
