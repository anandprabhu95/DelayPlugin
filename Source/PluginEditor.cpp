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
    g.fillAll (juce::Colours::black);
}

void DelayAudioProcessorEditor::resized()
{
    resizeGUI();
}

void DelayAudioProcessorEditor::createGUI()
{
    //====================================================================================
    gainSlider = std::make_unique<juce::Slider>("gainslider");
    addAndMakeVisible(gainSlider.get());
    gainSlider->setRange(0.0f, 1.0f, 0.1f);
    gainSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider->setSliderStyle(juce::Slider::Rotary);
    gainSlider->addListener(this);

    gainLabel = std::make_unique<juce::Label>("gainlabel", "Gain");
    addAndMakeVisible(gainLabel.get());
    gainLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    gainLabel->setJustificationType(juce::Justification::centred);
    gainLabel->setEditable(false, false, false);
    gainLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    gainLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    delayMsSlider = std::make_unique<juce::Slider>("delayslider");
    addAndMakeVisible(delayMsSlider.get());
    delayMsSlider->setRange(0.0f, 96000.0f, 10.f);
    delayMsSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayMsSlider->setSliderStyle(juce::Slider::Rotary);
    delayMsSlider->addListener(this);

    delayMsLabel = std::make_unique<juce::Label>("delaylabel", "Delay Time");
    addAndMakeVisible(delayMsLabel.get());
    delayMsLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    delayMsLabel->setJustificationType(juce::Justification::centred);
    delayMsLabel->setEditable(false, false, false);
    delayMsLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    delayMsLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    drywetSlider = std::make_unique<juce::Slider>("drywetslider");
    addAndMakeVisible(drywetSlider.get());
    drywetSlider->setRange(-1.0f, 1.0f, 0.01f);
    drywetSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    drywetSlider->setSliderStyle(juce::Slider::Rotary);
    drywetSlider->addListener(this);

    drywetLabel = std::make_unique<juce::Label>("drywetlabel", "Mix");
    addAndMakeVisible(drywetLabel.get());
    drywetLabel->setFont(juce::Font(10.00f, juce::Font::plain));
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
    int globalX = 50;
    int globalY = 50;
    gainSlider->setBounds(globalX + 0, globalY + 15, 100, 100);
    gainLabel->setBounds(globalX + 0, globalY + 0, 100, 24);

    delayMsSlider->setBounds(globalX + 100, globalY + 15, 100, 100);
    delayMsLabel->setBounds(globalX + 100, globalY + 0, 100, 24);

    drywetSlider->setBounds(globalX + 200, globalY + 15, 100, 100);
    drywetLabel->setBounds(globalX + 200, globalY + 0, 100, 24);
}

void DelayAudioProcessorEditor::paramAttacher()
{
    gainParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN", *gainSlider);
    delayMsParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS", *delayMsSlider);
    drywetParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", *drywetSlider);
}
void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved)
{
    
}

