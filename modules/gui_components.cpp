#include "gui_components.h"


void GUI::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
    juce::String componentName, juce::RangedAudioParameter* parameter)
{
    DBG("Attaching " << componentName);
    slider = std::make_unique<juce::Slider>(componentName);
    addAndMakeVisible(slider.get());
    slider->setSkewFactor(parameter->getNormalisableRange().skew);
    slider->setRange(parameter->getNormalisableRange().start, parameter->getNormalisableRange().end, parameter->getNormalisableRange().interval);
    slider->setSliderStyle(sliderStyle);
    setTextBox(slider);
    slider->addListener(this);
}

void GUI::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName)
{
    DBG("Attaching " << componentName);
    button = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(button.get());
    button->setSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    button->setClickingTogglesState(true);
}

void GUI::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, juce::Justification justification)
{
    DBG("Attaching " << componentName);
    label = std::make_unique<juce::Label>(componentName, textToDisplay);
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(LABEL_FONTSIZE, juce::Font::plain));
    label->setJustificationType(justification);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
}

void GUI::setTextBox(std::unique_ptr<juce::Slider>& slider)
{
    // Currently set to NoTextBox. The slider value is displayed as a Label component.
    juce::Colour outlinecolour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    outlinecolour.withAlpha(0.0f);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, true, TEXTBOX_WIDTH, TEXTBOX_HEIGHT);
    slider->setColour(juce::Slider::textBoxOutlineColourId, outlinecolour);
}

void GUI::initializeValueLabel(std::unique_ptr<juce::Label>& label)
{
    label = std::make_unique<juce::Label>("", "NULL");
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(VALUELABEL_FONTSIZE, juce::Font::plain));
    label->setJustificationType(juce::Justification::centred);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
}

void GUI::setValueLabel(std::unique_ptr<juce::Label>& label, std::unique_ptr<juce::Slider>& slider, juce::String suffix) const
{
    label->setText(slider->getTextFromValue(slider->getValue()) + " " + suffix, juce::dontSendNotification);
}
