#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    createGUI();
    paramAttacher();
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    destroyGUI();
}

//==============================================================================
void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::blueviolet);
}

void DelayAudioProcessorEditor::resized()
{
    resizeGUI();
}

void DelayAudioProcessorEditor::createGUI()
{
    //====================================================================================
    addAndMakeVisible(gainSlider = new juce::Slider("gainslider"));
    gainSlider->setRange(0.0f, 1.0f, 0.1f);
    gainSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider->setSliderStyle(juce::Slider::Rotary);
    gainSlider->addListener(this);

    addAndMakeVisible(gainLabel = new juce::Label("gainslider", "Gain"));
    gainLabel->setFont(juce::Font(15.00f, juce::Font::plain));
    gainLabel->setJustificationType(juce::Justification::centred);
    gainLabel->setEditable(false, false, false);
    gainLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    gainLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    addAndMakeVisible(delayMsSlider = new juce::Slider("delayslider"));
    delayMsSlider->setRange(0.0f, 96000.0f, 10.f);
    delayMsSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayMsSlider->setSliderStyle(juce::Slider::Rotary);
    delayMsSlider->addListener(this);

    addAndMakeVisible(delayMsLabel = new juce::Label("delayslider", "Delay Ms"));
    delayMsLabel->setFont(juce::Font(15.00f, juce::Font::plain));
    delayMsLabel->setJustificationType(juce::Justification::centred);
    delayMsLabel->setEditable(false, false, false);
    delayMsLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    delayMsLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    addAndMakeVisible(drywetSlider = new juce::Slider("drywetslider"));
    drywetSlider->setRange(-1.0f, 1.0f, 0.01f);
    drywetSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    drywetSlider->setSliderStyle(juce::Slider::Rotary);
    drywetSlider->addListener(this);

    addAndMakeVisible(drywetLabel = new juce::Label("drywetslider", "Dry/Wet"));
    drywetLabel->setFont(juce::Font(15.00f, juce::Font::plain));
    drywetLabel->setJustificationType(juce::Justification::centred);
    drywetLabel->setEditable(false, false, false);
    drywetLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    drywetLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);


    //=====================================================================================
    setSize(400, 300);
}

void DelayAudioProcessorEditor::destroyGUI()
{
    gainSlider = nullptr;
    gainLabel = nullptr;
    gainParamAttach = nullptr;

    delayMsSlider = nullptr;
    delayMsLabel = nullptr;
    delayMsParamAttach = nullptr;

    drywetSlider = nullptr;
    drywetLabel = nullptr;
    drywetParamAttach = nullptr;
}

void DelayAudioProcessorEditor::resizeGUI()
{
    gainSlider->setBounds(0, 0, 100, 100);
    gainLabel->setBounds(0, 101, 100, 24);

    delayMsSlider->setBounds(100, 0, 100, 100);
    delayMsLabel->setBounds(100, 101, 100, 24);

    drywetSlider->setBounds(200, 0, 100, 100);
    drywetLabel->setBounds(200, 101, 100, 24);
}

void DelayAudioProcessorEditor::paramAttacher()
{
    gainParamAttach = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.params, "FEEDBACKGAIN", *gainSlider);
    delayMsParamAttach = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.params, "DELAYMS", *delayMsSlider);
    drywetParamAttach = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.params, "DRYWET", *drywetSlider);
}
void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved)
{
    
}

