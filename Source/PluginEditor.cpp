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
    componentDisable();
    modifyDelaySliderForBpmSync();
}

void DelayAudioProcessorEditor::componentDisable()
{
    // Disable Slider2 when in mono modo.
    if (m_stereoDelayButton->getToggleState() == 0)
    {
        m_gainSlider2->setValue(m_gainSlider->getValue(), juce::dontSendNotification);
        m_gainSlider2->setEnabled(0);
        m_gainSlider2->setAlpha(0.5f);

        m_delayMsSlider2->setValue(m_delayMsSlider->getValue(), juce::dontSendNotification);
        m_delayMsSlider2->setEnabled(0);
        m_delayMsSlider2->setAlpha(0.5f);
    }
    else
    {
        m_gainSlider2->setEnabled(1);
        m_gainSlider2->setAlpha(1.0f);

        m_delayMsSlider2->setEnabled(1);
        m_delayMsSlider2->setAlpha(1.0f);
    }

    if (m_lfoButton->getToggleState() == 0)
    {
        m_lfoFreqSlider->setEnabled(0);
        m_lfoFreqSlider->setAlpha(0.5f);

        m_lfoAmtSlider->setEnabled(0);
        m_lfoAmtSlider->setAlpha(0.5f);
    }
    else
    {
        m_lfoFreqSlider->setEnabled(1);
        m_lfoFreqSlider->setAlpha(1.0f);

        m_lfoAmtSlider->setEnabled(1);
        m_lfoAmtSlider->setAlpha(1.0f);
    }
}

void DelayAudioProcessorEditor::resized()
{
    resizeGUI();
}

void DelayAudioProcessorEditor::createGUI()
{   
    DBG("Creating GUI");
    //====================================================================================
    DBG("Attaching GainSlider Left Channel");
    m_gainSlider = std::make_unique<juce::Slider>("gainslider");
    addAndMakeVisible(m_gainSlider.get());
    m_gainSlider->setRange(0.0f, 1.0f, 0.1f);
    m_gainSlider->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_gainSlider);
    m_gainSlider->addListener(this);

    DBG("Attaching GainLabel Left Channel");
    m_gainLabel = std::make_unique<juce::Label>("gainlabel", "Gain L");
    addAndMakeVisible(m_gainLabel.get());
    m_gainLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_gainLabel->setJustificationType(juce::Justification::centred);
    m_gainLabel->setEditable(false, false, false);
    m_gainLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_gainLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayMsSlider Left Channel");
    m_delayMsSlider = std::make_unique<juce::Slider>("delayslider");
    addAndMakeVisible(m_delayMsSlider.get());
    m_delayMsSlider->setRange(0.0f, MAX_DELAY_TIME, 0.0001f);
    m_delayMsSlider->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayMsSlider);
    m_delayMsSlider->addListener(this);

    DBG("Attaching DelayMsLabel Left Channel");
    m_delayMsLabel = std::make_unique<juce::Label>("delaylabel", "Delay Time L");
    addAndMakeVisible(m_delayMsLabel.get());
    m_delayMsLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_delayMsLabel->setJustificationType(juce::Justification::centred);
    m_delayMsLabel->setEditable(false, false, false);
    m_delayMsLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_delayMsLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayBpmSlider Left Channel");
    m_delayBpmSlider = std::make_unique<juce::Slider>("delaybpmslider");
    m_delayBpmSlider->setRange(0, 6, 1);
    m_delayBpmSlider->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayBpmSlider);
    m_delayBpmSlider->addListener(this);

    //====================================================================================
    DBG("Attaching GainSlider Right Channel");
    m_gainSlider2 = std::make_unique<juce::Slider>("gainslider2");
    addAndMakeVisible(m_gainSlider2.get());
    m_gainSlider2->setRange(0.0f, 1.0f, 0.1f);
    m_gainSlider2->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_gainSlider2);
    m_gainSlider2->addListener(this);

    DBG("Attaching GainLabel Right Channel");
    m_gainLabel2 = std::make_unique<juce::Label>("gainlabel2", "Gain R");
    addAndMakeVisible(m_gainLabel2.get());
    m_gainLabel2->setFont(juce::Font(10.00f, juce::Font::plain));
    m_gainLabel2->setJustificationType(juce::Justification::centred);
    m_gainLabel2->setEditable(false, false, false);
    m_gainLabel2->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_gainLabel2->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayMsSlider Right Channel");
    m_delayMsSlider2 = std::make_unique<juce::Slider>("delayslider2");
    addAndMakeVisible(m_delayMsSlider2.get());
    m_delayMsSlider2->setRange(0.0f, MAX_DELAY_TIME, 0.0001f);
    m_delayMsSlider2->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayMsSlider2);
    m_delayMsSlider2->addListener(this);

    DBG("Attaching DelayMsLabel Right Channel");
    m_delayMsLabel2 = std::make_unique<juce::Label>("delaylabel2", "Delay Time R");
    addAndMakeVisible(m_delayMsLabel2.get());
    m_delayMsLabel2->setFont(juce::Font(10.00f, juce::Font::plain));
    m_delayMsLabel2->setJustificationType(juce::Justification::centred);
    m_delayMsLabel2->setEditable(false, false, false);
    m_delayMsLabel2->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_delayMsLabel2->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DryWetSlider");
    m_drywetSlider = std::make_unique<juce::Slider>("drywetslider");
    addAndMakeVisible(m_drywetSlider.get());
    m_drywetSlider->setRange(-1.0f, 1.0f, 0.01f);
    m_drywetSlider->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_drywetSlider);
    m_drywetSlider->addListener(this);

    DBG("Attaching DryWetLabel");
    m_drywetLabel = std::make_unique<juce::Label>("drywetlabel", "Mix");
    addAndMakeVisible(m_drywetLabel.get());
    m_drywetLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_drywetLabel->setJustificationType(juce::Justification::centred);
    m_drywetLabel->setEditable(false, false, false);
    m_drywetLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_drywetLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching LfoButton");
    m_lfoButton = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_lfoButton.get());
    m_lfoButton->setSize(20, 20);
    m_lfoButton->setClickingTogglesState(true);

    DBG("Attaching LfoButtonLabel");
    m_lfoButtonLabel = std::make_unique<juce::Label>("lfobuttonlabel", "ENA");
    addAndMakeVisible(m_lfoButtonLabel.get());
    m_lfoButtonLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_lfoButtonLabel->setJustificationType(juce::Justification::left);
    m_lfoButtonLabel->setEditable(false, false, false);
    m_lfoButtonLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_lfoButtonLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching lfoFreqSlider");
    m_lfoFreqSlider = std::make_unique<juce::Slider>("lfofreqslider");
    addAndMakeVisible(m_lfoFreqSlider.get());
    m_lfoFreqSlider->setRange(1.0f, 10.0f, 0.01f);
    m_lfoFreqSlider->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_lfoFreqSlider);
    m_lfoFreqSlider->addListener(this);

    DBG("Attaching lfoFreqLabel");
    m_lfoFreqLabel = std::make_unique<juce::Label>("lfoFreqlabel", "LFO Freq");
    addAndMakeVisible(m_lfoFreqLabel.get());
    m_lfoFreqLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_lfoFreqLabel->setJustificationType(juce::Justification::centred);
    m_lfoFreqLabel->setEditable(false, false, false);
    m_lfoFreqLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_lfoFreqLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching lfoAmtSlider");
    m_lfoAmtSlider = std::make_unique<juce::Slider>("lfoamtslider");
    addAndMakeVisible(m_lfoAmtSlider.get());
    m_lfoAmtSlider->setRange(0.5f, 1.0f, 0.01f);
    m_lfoAmtSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    m_lfoAmtSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    m_lfoAmtSlider->addListener(this);

    DBG("Attaching lfoAmtLabel");
    m_lfoAmtLabel = std::make_unique<juce::Label>("lfoamtlabel", "AMT");
    addAndMakeVisible(m_lfoAmtLabel.get());
    m_lfoAmtLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_lfoAmtLabel->setJustificationType(juce::Justification::centred);
    m_lfoAmtLabel->setEditable(false, false, false);
    m_lfoAmtLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_lfoAmtLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching testReverbButton");
    m_testReverbButton = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_testReverbButton.get());
    m_testReverbButton->setSize(20, 20);
    m_testReverbButton->setClickingTogglesState(true);

    DBG("Attaching testReverbButtonLabel");
    m_testRvrbLabel = std::make_unique<juce::Label>("testreverbbuttonlabel", "Test Reverb");
    addAndMakeVisible(m_testRvrbLabel.get());
    m_testRvrbLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_testRvrbLabel->setJustificationType(juce::Justification::left);
    m_testRvrbLabel->setEditable(false, false, false);
    m_testRvrbLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_testRvrbLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching stereoDelayButton");
    m_stereoDelayButton = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_stereoDelayButton.get());
    m_stereoDelayButton->setSize(20, 20);
    m_stereoDelayButton->setClickingTogglesState(true);

    DBG("Attaching stereoDelayButtonLabel");
    m_stereoDelayLabel = std::make_unique<juce::Label>("stereodelaybuttonlabel", "Stereo");
    addAndMakeVisible(m_stereoDelayLabel.get());
    m_stereoDelayLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_stereoDelayLabel->setJustificationType(juce::Justification::left);
    m_stereoDelayLabel->setEditable(false, false, false);
    m_stereoDelayLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_stereoDelayLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching bpmSyncButton");
    m_bpmSyncButton = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_bpmSyncButton.get());
    m_bpmSyncButton->setSize(20, 20);
    m_bpmSyncButton->setClickingTogglesState(true);

    DBG("Attaching bpmSyncButtonLabel");
    m_bpmSyncButtonLabel = std::make_unique<juce::Label>("bpmsyncbuttonlabel", "BPM Sync");
    addAndMakeVisible(m_bpmSyncButtonLabel.get());
    m_bpmSyncButtonLabel->setFont(juce::Font(10.00f, juce::Font::plain));
    m_bpmSyncButtonLabel->setJustificationType(juce::Justification::left);
    m_bpmSyncButtonLabel->setEditable(false, false, false);
    m_bpmSyncButtonLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_bpmSyncButtonLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //=====================================================================================
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::silver);
    //getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red.darker(10));
    //getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::red.darker(5));
    setSize(WIDTH, HEIGHT);

    DBG("Created GUI");
}

void DelayAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider)
{
    juce::Colour outlinecolour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    outlinecolour.withAlpha(0.0f);
    slider->setTextBoxStyle(juce::Slider::TextBoxAbove, true, TEXTBOX_WIDTH, TEXTBOX_HEIGHT);
    slider->setColour(juce::Slider::textBoxOutlineColourId, outlinecolour);
}


void DelayAudioProcessorEditor::destroyGUI()
{   
    DBG("Destroying GUI");

    m_gainLabel = nullptr;
    m_gainParamAttach = nullptr;
    m_gainSlider = nullptr;
    
    m_delayMsLabel = nullptr;
    m_delayMsParamAttach = nullptr;
    m_delayMsSlider = nullptr;
    m_delayBpmParamAttach = nullptr;
    m_delayBpmSlider = nullptr;

    m_gainLabel2 = nullptr;
    m_gainParamAttach2 = nullptr;
    m_gainSlider2 = nullptr;

    m_delayMsLabel2 = nullptr;
    m_delayMsParamAttach2 = nullptr;
    m_delayMsSlider2 = nullptr;
   
    m_drywetLabel = nullptr;
    m_drywetParamAttach = nullptr;
    m_drywetSlider = nullptr;
    
    m_lfoButtonLabel = nullptr;
    m_lfoButtonParamAttach = nullptr;
    m_lfoButton = nullptr;

    m_lfoFreqLabel = nullptr;
    m_lfoFreqParamAttach = nullptr;
    m_lfoFreqSlider = nullptr;

    m_lfoAmtLabel = nullptr;
    m_lfoAmtParamAttach = nullptr;
    m_lfoAmtSlider = nullptr;

    m_testRvrbLabel = nullptr;
    m_testRvrbButParamAttach = nullptr;
    m_testReverbButton = nullptr;

    m_stereoDelayLabel = nullptr;
    m_stereoDelayButParamAttach = nullptr;
    m_stereoDelayButton = nullptr;

    m_bpmSyncButtonLabel = nullptr;
    m_bpmSyncButParamAttach = nullptr;
    m_bpmSyncButton = nullptr;

    DBG("Destroyed GUI");
}

void DelayAudioProcessorEditor::resizeGUI()
{   
    DBG("Resizing GUI");

    m_gainSlider->setBounds(GLOBAL_X + FB_GAIN_SLIDER_X, GLOBAL_Y + FB_GAIN_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabel->setBounds(GLOBAL_X + FB_GAIN_LABEL_X, GLOBAL_Y + FB_GAIN_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSlider->setBounds(GLOBAL_X + DELAYMS_SLIDER_X, GLOBAL_Y + DELAYMS_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayBpmSlider->setBounds(GLOBAL_X + DELAYBPM_SLIDER_X, GLOBAL_Y + DELAYBPM_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabel->setBounds(GLOBAL_X + DELAYMS_LABEL_X, GLOBAL_Y + DELAYMS_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_gainSlider2->setBounds(GLOBAL_X + FB_GAIN_SLIDER_X2, GLOBAL_Y + FB_GAIN_SLIDER_Y2, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabel2->setBounds(GLOBAL_X + FB_GAIN_LABEL_X2, GLOBAL_Y + FB_GAIN_LABEL_Y2, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSlider2->setBounds(GLOBAL_X + DELAYMS_SLIDER_X2, GLOBAL_Y + DELAYMS_SLIDER_Y2, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabel2->setBounds(GLOBAL_X + DELAYMS_LABEL_X2, GLOBAL_Y + DELAYMS_LABEL_Y2, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_drywetSlider->setBounds(GLOBAL_X + MIX_SLIDER_X, GLOBAL_Y + MIX_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_drywetLabel->setBounds(GLOBAL_X + MIX_LABEL_X, GLOBAL_Y + MIX_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_lfoButton->setBounds(GLOBAL_X + LFOENA_BUT_X, GLOBAL_Y + LFOENA_BUT_Y, LFOENA_BUT_WIDTH, LFOENA_BUT_HEIGHT);
    m_lfoButtonLabel->setBounds(GLOBAL_X + LFOENA_LABEL_X, GLOBAL_Y + LFOENA_LABEL_Y, LFOENA_BUT_LAB_WIDTH, LFOENA_BUT_LAB_HEIGHT);

    m_lfoFreqSlider->setBounds(GLOBAL_X + LFOFREQ_SLIDER_X, GLOBAL_Y + LFOFREQ_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_lfoFreqLabel->setBounds(GLOBAL_X + LFOFREQ_LABEL_X, GLOBAL_Y + LFOFREQ_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);
    
    m_lfoAmtSlider->setBounds(GLOBAL_X + LFOAMT_SLIDER_X, GLOBAL_Y + LFOAMT_SLIDER_Y, LIN_SLIDER_WIDTH, LIN_SLIDER_HEIGHT);
    m_lfoAmtLabel->setBounds(GLOBAL_X + LFOAMT_LABEL_X, GLOBAL_Y + LFOAMT_LABEL_Y, LFOAMT_LAB_WIDTH, LFOAMT_LAB_HEIGHT);

    m_testReverbButton->setBounds(GLOBAL_X + TESTRVRB_BUT_X, GLOBAL_Y + TESTRVRB_BUT_Y, TESTRVRB_BUT_WIDTH, TESTRVRB_BUT_HEIGHT);
    m_testRvrbLabel->setBounds(GLOBAL_X + TESTRVRB_LABEL_X, GLOBAL_Y + TESTRVRB_LABEL_Y, TESTRVRB_BUT_LAB_WIDTH, TESTRVRB_BUT_LAB_HEIGHT);

    m_stereoDelayButton->setBounds(GLOBAL_X + STRODEL_BUT_X, GLOBAL_Y + STRODEL_BUT_Y, STRODEL_BUT_WIDTH, STRODEL_BUT_HEIGHT);
    m_stereoDelayLabel->setBounds(GLOBAL_X + STRODEL_LABEL_X, GLOBAL_Y + STRODEL_LABEL_Y, STRODEL_BUT_LAB_WIDTH, STRODEL_BUT_LAB_HEIGHT);

    m_bpmSyncButton->setBounds(GLOBAL_X + BPMSYNC_BUT_X, GLOBAL_Y + BPMSYNC_BUT_Y, BPMSYNC_BUT_WIDTH, BPMSYNC_BUT_HEIGHT);
    m_bpmSyncButtonLabel->setBounds(GLOBAL_X + BPMSYNC_LABEL_X, GLOBAL_Y + BPMSYNC_LABEL_Y, BPMSYNC_BUT_LAB_WIDTH, BPMSYNC_BUT_LAB_HEIGHT);
    
    DBG("Resized GUI");
}

void DelayAudioProcessorEditor::paramAttacher()
{
    DBG("Attaching Params");
    m_gainParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN", *m_gainSlider);
    m_gainParamAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN2", *m_gainSlider2);
    m_delayMsParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS", *m_delayMsSlider);
    m_delayMsParamAttach2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS2", *m_delayMsSlider2);
    m_delayBpmParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYBPM", *m_delayBpmSlider);
    m_drywetParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", *m_drywetSlider);
    m_lfoButtonParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "LFOENA", *m_lfoButton);
    m_lfoFreqParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOFREQ", *m_lfoFreqSlider);
    m_lfoAmtParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOAMT", *m_lfoAmtSlider);
    m_testRvrbButParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "TESTRVRB", *m_testReverbButton);
    m_stereoDelayButParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "STRODEL", *m_stereoDelayButton);
    m_bpmSyncButParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "BPMSYNC", *m_bpmSyncButton);

    DBG("Attached Params");
}

void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}

//=================================================================================

void DelayAudioProcessorEditor::paintBackground(juce::Graphics& g)
{
    juce::Colour colour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    juce::ColourGradient cg = juce::ColourGradient::vertical(colour, 0, colour.darker(1.5f), HEIGHT);
    g.setGradientFill(cg);
    g.fillAll();

    g.setFont(9.0f);
    g.setColour(juce::Colours::white);
    g.drawText(VERSION, 0, HEIGHT - 10, WIDTH, 10, juce::Justification::right, true);
    g.setColour(juce::Colours::white);
    g.drawLine(0, HEIGHT - 10, WIDTH, HEIGHT - 10, 0.5);
}

void DelayAudioProcessorEditor::modifyDelaySliderForBpmSync()
{
    if (m_bpmSyncButton->getToggleState() == 1)
    {
        m_delayMsSlider->setVisible(false);
        addAndMakeVisible(m_delayBpmSlider.get());
    }
    else
    {
        m_delayBpmSlider->setVisible(false);
        addAndMakeVisible(m_delayMsSlider.get());
    }
}

