#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor(ReverbAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::silver); // Thumb Color
    setSize(WIDTH, HEIGHT); // Plugin Window Size

    createSlider(m_drywetSlider, juce::Slider::Rotary, "drywetSlider", "DRYWET");
    paramAttacher();

    createLabel(m_drywetLabel, "drywetlabel", "Mix", juce::Justification::centred);
}


ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
}


//==============================================================================
void ReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (9.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

//==============================================================================
void ReverbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

}

void ReverbAudioProcessorEditor::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
    juce::String componentName, juce::String propertyID)
{
    DBG("Attaching " << componentName);
    slider = std::make_unique<juce::Slider>(componentName);
    addAndMakeVisible(slider.get());
    juce::RangedAudioParameter* prop = audioProcessor.params.getParameter(propertyID);
    slider->setSkewFactor(prop->getNormalisableRange().skew);
    slider->setRange(prop->getNormalisableRange().start, prop->getNormalisableRange().end, prop->getNormalisableRange().interval);
    slider->setSliderStyle(sliderStyle);
    setTextBox(slider);
    slider->addListener(this);
}

void ReverbAudioProcessorEditor::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName)
{
    DBG("Attaching " << componentName);
    button = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(button.get());
    button->setSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    button->setClickingTogglesState(true);
}


void ReverbAudioProcessorEditor::paramAttacher()
{
    DBG("Attaching params.");
    m_drywetParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", *m_drywetSlider);
    m_spaceParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "SPACE", *m_spaceSlider);
    m_decayParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DECAY", *m_decaySlider);
}

void ReverbAudioProcessorEditor::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, juce::Justification justification)
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

void ReverbAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider)
{
    // Currently set to NoTextBox. The slider value is displayed as a Label component.
    juce::Colour outlinecolour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    outlinecolour.withAlpha(0.0f);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, true, TEXTBOX_WIDTH, TEXTBOX_HEIGHT);
    slider->setColour(juce::Slider::textBoxOutlineColourId, outlinecolour);
}

