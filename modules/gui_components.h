#include <JuceHeader.h>


#define SLIDER_WIDTH					100
#define SLIDER_HEIGHT					100

#define LIN_SLIDER_WIDTH				70
#define LIN_SLIDER_HEIGHT				24

#define SLIDER_LABEL_WIDTH				100
#define SLIDER_LABEL_HEIGHT				24

#define TEXTBOX_WIDTH					40
#define TEXTBOX_HEIGHT					20

#define BUTTON_WIDTH					20
#define BUTTON_HEIGHT					20

#define GLOBAL_X						100
#define GLOBAL_Y						50

#define LABEL_FONTSIZE					10.0f
#define VALUELABEL_FONTSIZE				14.0f

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

