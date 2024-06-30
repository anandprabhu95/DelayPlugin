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
    paintBackground(g);

    g.setFont(9.0f);
    g.setColour(juce::Colours::white);
    g.drawText(VERSION, 0, HEIGHT-10, WIDTH, 10, juce::Justification::right, true);

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
    lfoButtonLabel = std::make_unique<juce::Label>("lfobuttonlabel", "ENA");
    addAndMakeVisible(lfoButtonLabel.get());
    lfoButtonLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    lfoButtonLabel->setJustificationType(juce::Justification::left);
    lfoButtonLabel->setEditable(false, false, false);
    lfoButtonLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    lfoButtonLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching lfoFreqSlider");
    lfoFreqSlider = std::make_unique<juce::Slider>("lfofreqslider");
    addAndMakeVisible(lfoFreqSlider.get());
    lfoFreqSlider->setRange(1.0f, 10.0f, 0.01f);
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

    //====================================================================================
    DBG("Attaching lfoAmtSlider");
    lfoAmtSlider = std::make_unique<juce::Slider>("lfoamtslider");
    addAndMakeVisible(lfoAmtSlider.get());
    lfoAmtSlider->setRange(0.5f, 1.0f, 0.01f);
    lfoAmtSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lfoAmtSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    lfoAmtSlider->addListener(this);

    DBG("Attaching lfoAmtLabel");
    lfoAmtLabel = std::make_unique<juce::Label>("lfoamtlabel", "AMT");
    addAndMakeVisible(lfoAmtLabel.get());
    lfoAmtLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    lfoAmtLabel->setJustificationType(juce::Justification::centred);
    lfoAmtLabel->setEditable(false, false, false);
    lfoAmtLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    lfoAmtLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //=====================================================================================
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::silver);
    //getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red.darker(10));
    //getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::red.darker(5));
    setSize(WIDTH, HEIGHT);

    DBG("Created GUI");
}

void DelayAudioProcessorEditor::destroyGUI()
{   
    DBG("Destroying GUI");

    gainLabel = nullptr;
    gainParamAttach = nullptr;
    gainSlider = nullptr;
    
    delayMsLabel = nullptr;
    delayMsParamAttach = nullptr;
    delayMsSlider = nullptr;
   
    drywetLabel = nullptr;
    drywetParamAttach = nullptr;
    drywetSlider = nullptr;
    
    lfoButtonLabel = nullptr;
    lfoButtonParamAttach = nullptr;
    lfoButton = nullptr;

    lfoFreqLabel = nullptr;
    lfoFreqParamAttach = nullptr;
    lfoFreqSlider = nullptr;

    lfoAmtLabel = nullptr;
    lfoAmtParamAttach = nullptr;
    lfoAmtSlider = nullptr;

    DBG("Destroyed GUI");
}

void DelayAudioProcessorEditor::resizeGUI()
{   
    DBG("Resizing GUI");

    gainSlider->setBounds(GLOBAL_X + FB_GAIN_SLIDER_X, GLOBAL_Y + FB_GAIN_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    gainLabel->setBounds(GLOBAL_X + FB_GAIN_LABEL_X, GLOBAL_Y + FB_GAIN_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    delayMsSlider->setBounds(GLOBAL_X + DELAYMS_SLIDER_X, GLOBAL_Y + DELAYMS_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    delayMsLabel->setBounds(GLOBAL_X + DELAYMS_LABEL_X, GLOBAL_Y + DELAYMS_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    drywetSlider->setBounds(GLOBAL_X + MIX_SLIDER_X, GLOBAL_Y + MIX_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    drywetLabel->setBounds(GLOBAL_X + MIX_LABEL_X, GLOBAL_Y + MIX_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    lfoButton->setBounds(GLOBAL_X + LFOENA_BUT_X, GLOBAL_Y + LFOENA_BUT_Y, LFOENA_BUT_WIDTH, LFOENA_BUT_HEIGHT);
    lfoButtonLabel->setBounds(GLOBAL_X + LFOENA_LABEL_X, GLOBAL_Y + LFOENA_LABEL_Y, LFOENA_BUT_LAB_WIDTH, LFOENA_BUT_LAB_HEIGHT);

    lfoFreqSlider->setBounds(GLOBAL_X + LFOFREQ_SLIDER_X, GLOBAL_Y + LFOFREQ_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    lfoFreqLabel->setBounds(GLOBAL_X + LFOFREQ_LABEL_X, GLOBAL_Y + LFOFREQ_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);
    
    lfoAmtSlider->setBounds(GLOBAL_X + LFOAMT_SLIDER_X, GLOBAL_Y + LFOAMT_SLIDER_Y, LIN_SLIDER_WIDTH, LIN_SLIDER_HEIGHT);
    lfoAmtLabel->setBounds(GLOBAL_X + LFOAMT_LABEL_X, GLOBAL_Y + LFOAMT_LABEL_Y, LFOAMT_LAB_WIDTH, LFOAMT_LAB_HEIGHT);
    
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
    lfoAmtParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOAMT", *lfoAmtSlider);
    DBG("Attached Params");
}

void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved)
{
    
}

//=================================================================================

void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g)
{
    juce::Colour colour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    juce::ColourGradient cg = juce::ColourGradient::vertical(colour, 0, colour.darker(1.5f), HEIGHT);
    g.setGradientFill(cg);
    g.fillAll();
}

