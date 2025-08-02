#include <JuceHeader.h>
#include "../Source/PluginProcessor.h"


class GUI : public juce::AudioProcessorEditor, 
            public juce::Slider::Listener           
{
public:
    void GUI::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
                        juce::String componentName, juce::RangedAudioParameter* parameter);
    void GUI::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName);
    void GUI::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, 
                        juce::Justification justification);
    void GUI::setTextBox(std::unique_ptr<juce::Slider>& slider);
    void GUI::initializeValueLabel(std::unique_ptr<juce::Label>& label);
    void GUI::setValueLabel(std::unique_ptr<juce::Label>& label, std::unique_ptr<juce::Slider>& slider, juce::String suffix) const;

    enum TrackStatus { DEFAULT, CHANGED, UNCHANGED };

    class GUI::ButtonStatusUpdate
    {
    public:
        bool value;
        enum TrackStatus status;
        ButtonStatusUpdate()
        {
            value = false;
            status = DEFAULT;
        }

        TrackStatus check(std::unique_ptr<juce::ToggleButton>& button)
        {
            // WARNING: Do not call this function on the same object more than once in an execution cycle.
            bool newValue = button->getToggleState();
            if (newValue != value || status == DEFAULT)
            {
                status = CHANGED;
                value = newValue;
            }
            else
            {
                status = UNCHANGED;
            }
            return status;
        }
    };  
};

