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

    g.setFont(9.0f);
    g.setColour(juce::Colours::white);
    g.drawText("Frecurse v1.0.0", 0, HEIGHT-10, WIDTH, 10, juce::Justification::right, true);

    g.setColour(juce::Colours::white);
    g.drawLine(0, HEIGHT-10, WIDTH, HEIGHT-10, 0.5);
}

void DelayAudioProcessorEditor::resized()
{
    resizeGUI();
}

void DelayAudioProcessorEditor::createGUI()
{   
    DBG("Creating GUI");
    //====================================================================================
    DBG("Attaching GainSlider");
    gainSlider = std::make_unique<juce::Slider>("gainslider");
    addAndMakeVisible(gainSlider.get());
    gainSlider->setRange(0.0f, 1.0f, 0.1f);
    gainSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider->setSliderStyle(juce::Slider::Rotary);
    gainSlider->addListener(this);

    DBG("Attaching GainLabel");
    gainLabel = std::make_unique<juce::Label>("gainlabel", "Gain");
    addAndMakeVisible(gainLabel.get());
    gainLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    gainLabel->setJustificationType(juce::Justification::centred);
    gainLabel->setEditable(false, false, false);
    gainLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    gainLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayMsSlider");
    delayMsSlider = std::make_unique<juce::Slider>("delayslider");
    addAndMakeVisible(delayMsSlider.get());
    delayMsSlider->setRange(0.0f, 96000.0f, 10.f);
    delayMsSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayMsSlider->setSliderStyle(juce::Slider::Rotary);
    delayMsSlider->addListener(this);

    DBG("Attaching DelayMsLabel");
    delayMsLabel = std::make_unique<juce::Label>("delaylabel", "Delay Time");
    addAndMakeVisible(delayMsLabel.get());
    delayMsLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    delayMsLabel->setJustificationType(juce::Justification::centred);
    delayMsLabel->setEditable(false, false, false);
    delayMsLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    delayMsLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DryWetSlider");
    drywetSlider = std::make_unique<juce::Slider>("drywetslider");
    addAndMakeVisible(drywetSlider.get());
    drywetSlider->setRange(-1.0f, 1.0f, 0.01f);
    drywetSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    drywetSlider->setSliderStyle(juce::Slider::Rotary);
    drywetSlider->addListener(this);

    DBG("Attaching DryWetLabel");
    drywetLabel = std::make_unique<juce::Label>("drywetlabel", "Mix");
    addAndMakeVisible(drywetLabel.get());
    drywetLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    drywetLabel->setJustificationType(juce::Justification::centred);
    drywetLabel->setEditable(false, false, false);
    drywetLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    drywetLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching LfoButton");
    lfoButton = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(lfoButton.get());
    lfoButton->setSize(20, 20);
    lfoButton->setClickingTogglesState(true);

    DBG("Attaching LfoButtonLabel");
    lfoButtonLabel = std::make_unique<juce::Label>("lfobuttonlabel", "LFO");
    addAndMakeVisible(lfoButtonLabel.get());
    lfoButtonLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    lfoButtonLabel->setJustificationType(juce::Justification::centred);
    lfoButtonLabel->setEditable(false, false, false);
    lfoButtonLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    lfoButtonLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching lfoFreqSlider");
    lfoFreqSlider = std::make_unique<juce::Slider>("lfofreqslider");
    addAndMakeVisible(lfoFreqSlider.get());
    lfoFreqSlider->setRange(-1.0f, 1.0f, 0.01f);
    lfoFreqSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lfoFreqSlider->setSliderStyle(juce::Slider::Rotary);
    lfoFreqSlider->addListener(this);

    DBG("Attaching lfoFreqLabel");
    lfoFreqLabel = std::make_unique<juce::Label>("lfoFreqlabel", "LFO Freq");
    addAndMakeVisible(lfoFreqLabel.get());
    lfoFreqLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    lfoFreqLabel->setJustificationType(juce::Justification::centred);
    lfoFreqLabel->setEditable(false, false, false);
    lfoFreqLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    lfoFreqLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //=====================================================================================
    DBG("Setting Size");
    setSize(WIDTH, HEIGHT);

    DBG("Created GUI");
}

void DelayAudioProcessorEditor::destroyGUI()
{   
    DBG("Destroying GUI");

    //gainSlider = nullptr;
    gainLabel = nullptr;
    gainParamAttach = nullptr;
    
    //delayMsSlider = nullptr;
    delayMsLabel = nullptr;
    delayMsParamAttach = nullptr;
    
    //drywetSlider = nullptr;
    drywetLabel = nullptr;
    drywetParamAttach = nullptr;

    lfoButton = nullptr;
    lfoButtonLabel = nullptr;

    lfoFreqLabel = nullptr;
    lfoFreqParamAttach = nullptr;

    DBG("Destroyed GUI");
}

void DelayAudioProcessorEditor::resizeGUI()
{   
    DBG("Resizing GUI");
    int globalX = 50;
    int globalY = 50;

    gainSlider->setBounds(globalX + 0, globalY + 15, 100, 100);
    gainLabel->setBounds(globalX + 0, globalY + 0, 100, 24);

    delayMsSlider->setBounds(globalX + 100, globalY + 15, 100, 100);
    delayMsLabel->setBounds(globalX + 100, globalY + 0, 100, 24);

    drywetSlider->setBounds(globalX + 200, globalY + 15, 100, 100);
    drywetLabel->setBounds(globalX + 200, globalY + 0, 100, 24);

    lfoButton->setBounds(globalX + 137.5, globalY + 120, 40, 40);
    lfoButtonLabel->setBounds(globalX + 145, globalY + 120, 50, 40);

    lfoFreqSlider->setBounds(globalX + 100, globalY + 160, 100, 100);
    lfoFreqLabel->setBounds(globalX + 100, globalY + 145, 100, 24);

    DBG("Resized GUI");
}

void DelayAudioProcessorEditor::paramAttacher()
{
    DBG("Attaching Params");
    gainParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN", *gainSlider);
    delayMsParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS", *delayMsSlider);
    drywetParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", *drywetSlider);
    lfoButtonParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "LFOENA", *lfoButton);
    lfoFreqParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOFREQ", *lfoFreqSlider);
    DBG("Attached Params");
}
void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved)
{
    
}

