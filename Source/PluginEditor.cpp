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
        m_gainSliderRight->setValue(m_gainSliderLeft->getValue(), juce::dontSendNotification);
        m_gainSliderRight->setEnabled(0);
        m_gainSliderRight->setAlpha(0.5f);

        m_delayMsSliderRight->setValue(m_delayMsSliderLeft->getValue(), juce::dontSendNotification);
        m_delayMsSliderRight->setEnabled(0);
        m_delayMsSliderRight->setAlpha(0.5f);
    }
    else
    {
        m_gainSliderRight->setEnabled(1);
        m_gainSliderRight->setAlpha(1.0f);

        m_delayMsSliderRight->setEnabled(1);
        m_delayMsSliderRight->setAlpha(1.0f);
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
    m_gainSliderLeft = std::make_unique<juce::Slider>("gainsliderleft");
    addAndMakeVisible(m_gainSliderLeft.get());
    m_gainSliderLeft->setRange(0.0f, 1.0f, 0.1f);
    m_gainSliderLeft->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_gainSliderLeft);
    m_gainSliderLeft->addListener(this);

    DBG("Attaching GainLabel Left Channel");
    m_gainLabelLeft = std::make_unique<juce::Label>("gainlabelleft", "Gain L");
    addAndMakeVisible(m_gainLabelLeft.get());
    m_gainLabelLeft->setFont(juce::Font(10.00f, juce::Font::plain));
    m_gainLabelLeft->setJustificationType(juce::Justification::centred);
    m_gainLabelLeft->setEditable(false, false, false);
    m_gainLabelLeft->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_gainLabelLeft->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayMsSlider Left Channel");
    m_delayMsSliderLeft = std::make_unique<juce::Slider>("delaysliderleft");
    addAndMakeVisible(m_delayMsSliderLeft.get());
    m_delayMsSliderLeft->setRange(0.0f, MAX_DELAY_TIME, 0.0001f);
    m_delayMsSliderLeft->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayMsSliderLeft);
    m_delayMsSliderLeft->addListener(this);

    DBG("Attaching DelayMsLabel Left Channel");
    m_delayMsLabelLeft = std::make_unique<juce::Label>("delaylabelleft", "Delay Time L");
    addAndMakeVisible(m_delayMsLabelLeft.get());
    m_delayMsLabelLeft->setFont(juce::Font(10.00f, juce::Font::plain));
    m_delayMsLabelLeft->setJustificationType(juce::Justification::centred);
    m_delayMsLabelLeft->setEditable(false, false, false);
    m_delayMsLabelLeft->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_delayMsLabelLeft->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayBpmSlider Left Channel");
    m_delayBpmSliderLeft = std::make_unique<juce::Slider>("delaybpmsliderleft");
    m_delayBpmSliderLeft->setRange(0, 6, 1);
    m_delayBpmSliderLeft->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayBpmSliderLeft);
    m_delayBpmSliderLeft->addListener(this);

    //====================================================================================
    DBG("Attaching GainSlider Right Channel");
    m_gainSliderRight = std::make_unique<juce::Slider>("gainsliderright");
    addAndMakeVisible(m_gainSliderRight.get());
    m_gainSliderRight->setRange(0.0f, 1.0f, 0.1f);
    m_gainSliderRight->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_gainSliderRight);
    m_gainSliderRight->addListener(this);

    DBG("Attaching GainLabel Right Channel");
    m_gainLabelRight = std::make_unique<juce::Label>("gainlabelright", "Gain R");
    addAndMakeVisible(m_gainLabelRight.get());
    m_gainLabelRight->setFont(juce::Font(10.00f, juce::Font::plain));
    m_gainLabelRight->setJustificationType(juce::Justification::centred);
    m_gainLabelRight->setEditable(false, false, false);
    m_gainLabelRight->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_gainLabelRight->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayMsSlider Right Channel");
    m_delayMsSliderRight = std::make_unique<juce::Slider>("delaysliderright");
    addAndMakeVisible(m_delayMsSliderRight.get());
    m_delayMsSliderRight->setRange(0.0f, MAX_DELAY_TIME, 0.0001f);
    m_delayMsSliderRight->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayMsSliderRight);
    m_delayMsSliderRight->addListener(this);

    DBG("Attaching DelayMsLabel Right Channel");
    m_delayMsLabelRight = std::make_unique<juce::Label>("delaylabelright", "Delay Time R");
    addAndMakeVisible(m_delayMsLabelRight.get());
    m_delayMsLabelRight->setFont(juce::Font(10.00f, juce::Font::plain));
    m_delayMsLabelRight->setJustificationType(juce::Justification::centred);
    m_delayMsLabelRight->setEditable(false, false, false);
    m_delayMsLabelRight->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_delayMsLabelRight->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching DelayBpmSlider Right Channel");
    m_delayBpmSliderRight = std::make_unique<juce::Slider>("delaybpmsliderright");
    m_delayBpmSliderRight->setRange(0, 6, 1);
    m_delayBpmSliderRight->setSliderStyle(juce::Slider::Rotary);
    setTextBox(m_delayBpmSliderRight);
    m_delayBpmSliderRight->addListener(this);

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
    DBG("Attaching bpmSyncButton for Left channel");
    m_bpmSyncButtonLeft = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_bpmSyncButtonLeft.get());
    m_bpmSyncButtonLeft->setSize(20, 20);
    m_bpmSyncButtonLeft->setClickingTogglesState(true);

    DBG("Attaching bpmSyncButtonLabel for Left channel");
    m_bpmSyncButtonLabelLeft = std::make_unique<juce::Label>("bpmsyncbuttonlabelleft", "BPM Sync");
    addAndMakeVisible(m_bpmSyncButtonLabelLeft.get());
    m_bpmSyncButtonLabelLeft->setFont(juce::Font(10.00f, juce::Font::plain));
    m_bpmSyncButtonLabelLeft->setJustificationType(juce::Justification::left);
    m_bpmSyncButtonLabelLeft->setEditable(false, false, false);
    m_bpmSyncButtonLabelLeft->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_bpmSyncButtonLabelLeft->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

    //====================================================================================
    DBG("Attaching bpmSyncButton for Right channel");
    m_bpmSyncButtonRight = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(m_bpmSyncButtonRight.get());
    m_bpmSyncButtonRight->setSize(20, 20);
    m_bpmSyncButtonRight->setClickingTogglesState(true);

    DBG("Attaching bpmSync2ButtonLabel for Right Channel");
    m_bpmSyncButtonLabelRight = std::make_unique<juce::Label>("bpmsync2buttonlabelright", "BPM Sync");
    addAndMakeVisible(m_bpmSyncButtonLabelRight.get());
    m_bpmSyncButtonLabelRight->setFont(juce::Font(10.00f, juce::Font::plain));
    m_bpmSyncButtonLabelRight->setJustificationType(juce::Justification::left);
    m_bpmSyncButtonLabelRight->setEditable(false, false, false);
    m_bpmSyncButtonLabelRight->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    m_bpmSyncButtonLabelRight->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);

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

    m_gainLabelLeft = nullptr;
    m_gainParamAttachLeft = nullptr;
    m_gainSliderLeft = nullptr;
    
    m_delayMsLabelLeft = nullptr;
    m_delayMsParamAttachLeft = nullptr;
    m_delayMsSliderLeft = nullptr;
    m_delayBpmParamAttachLeft = nullptr;
    m_delayBpmSliderLeft = nullptr;

    m_gainLabelRight = nullptr;
    m_gainParamAttachRight = nullptr;
    m_gainSliderRight = nullptr;

    m_delayMsLabelRight = nullptr;
    m_delayMsParamAttachRight = nullptr;
    m_delayMsSliderRight = nullptr;
    m_delayBpmParamAttachRight = nullptr;
    m_delayBpmSliderRight = nullptr;
   
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

    m_bpmSyncButtonLabelLeft = nullptr;
    m_bpmSyncButParamAttachLeft = nullptr;
    m_bpmSyncButtonLeft = nullptr;

    m_bpmSyncButtonLabelRight = nullptr;
    m_bpmSyncButParamAttachRight = nullptr;
    m_bpmSyncButtonRight = nullptr;

    DBG("Destroyed GUI");
}

void DelayAudioProcessorEditor::resizeGUI()
{   
    DBG("Resizing GUI");

    m_gainSliderLeft->setBounds(GLOBAL_X + FB_GAIN_SLIDER_X, GLOBAL_Y + FB_GAIN_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabelLeft->setBounds(GLOBAL_X + FB_GAIN_LABEL_X, GLOBAL_Y + FB_GAIN_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSliderLeft->setBounds(GLOBAL_X + DELAYMS_SLIDER_X, GLOBAL_Y + DELAYMS_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayBpmSliderLeft->setBounds(GLOBAL_X + DELAYBPM_SLIDER_X, GLOBAL_Y + DELAYBPM_SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabelLeft->setBounds(GLOBAL_X + DELAYMS_LABEL_X, GLOBAL_Y + DELAYMS_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_gainSliderRight->setBounds(GLOBAL_X + FB_GAIN_SLIDER_X2, GLOBAL_Y + FB_GAIN_SLIDER_Y2, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabelRight->setBounds(GLOBAL_X + FB_GAIN_LABEL_X2, GLOBAL_Y + FB_GAIN_LABEL_Y2, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSliderRight->setBounds(GLOBAL_X + DELAYMS_SLIDER_X2, GLOBAL_Y + DELAYMS_SLIDER_Y2, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayBpmSliderRight->setBounds(GLOBAL_X + DELAYBPM_SLIDER_X2, GLOBAL_Y + DELAYBPM_SLIDER_Y2, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabelRight->setBounds(GLOBAL_X + DELAYMS_LABEL_X2, GLOBAL_Y + DELAYMS_LABEL_Y2, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

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

    m_bpmSyncButtonLeft->setBounds(GLOBAL_X + BPMSYNC_BUT_X, GLOBAL_Y + BPMSYNC_BUT_Y, BPMSYNC_BUT_WIDTH, BPMSYNC_BUT_HEIGHT);
    m_bpmSyncButtonLabelLeft->setBounds(GLOBAL_X + BPMSYNC_LABEL_X, GLOBAL_Y + BPMSYNC_LABEL_Y, BPMSYNC_BUT_LAB_WIDTH, BPMSYNC_BUT_LAB_HEIGHT);

    m_bpmSyncButtonRight->setBounds(GLOBAL_X + BPMSYNC2_BUT_X, GLOBAL_Y + BPMSYNC2_BUT_Y, BPMSYNC2_BUT_WIDTH, BPMSYNC2_BUT_HEIGHT);
    m_bpmSyncButtonLabelRight->setBounds(GLOBAL_X + BPMSYNC2_LABEL_X, GLOBAL_Y + BPMSYNC2_LABEL_Y, BPMSYNC2_BUT_LAB_WIDTH, BPMSYNC2_BUT_LAB_HEIGHT);
    
    DBG("Resized GUI");
}

void DelayAudioProcessorEditor::paramAttacher()
{
    DBG("Attaching Params");
    m_gainParamAttachLeft = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN_LEFT", *m_gainSliderLeft);
    m_gainParamAttachRight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FEEDBACKGAIN_RIGHT", *m_gainSliderRight);
    m_delayMsParamAttachLeft = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS_LEFT", *m_delayMsSliderLeft);
    m_delayMsParamAttachRight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYMS_RIGHT", *m_delayMsSliderRight);
    m_delayBpmParamAttachLeft = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYBPM_LEFT", *m_delayBpmSliderLeft);
    m_delayBpmParamAttachRight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DELAYBPM_RIGHT", *m_delayBpmSliderRight);
    m_drywetParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "DRYWET", *m_drywetSlider);
    m_lfoButtonParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "LFOENA", *m_lfoButton);
    m_lfoFreqParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOFREQ", *m_lfoFreqSlider);
    m_lfoAmtParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "LFOAMT", *m_lfoAmtSlider);
    m_testRvrbButParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "TESTRVRB", *m_testReverbButton);
    m_stereoDelayButParamAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "STRODEL", *m_stereoDelayButton);
    m_bpmSyncButParamAttachLeft = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "BPMSYNC_LEFT", *m_bpmSyncButtonLeft);
    m_bpmSyncButParamAttachRight = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.params, "BPMSYNC_RIGHT", *m_bpmSyncButtonRight);

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
    if (m_bpmSyncButtonLeft->getToggleState() == 1)
    {
        m_delayMsSliderLeft->setVisible(false);
        addAndMakeVisible(m_delayBpmSliderLeft.get());
    }
    else
    {
        m_delayBpmSliderLeft->setVisible(false);
        addAndMakeVisible(m_delayMsSliderLeft.get());
    }

    if (m_bpmSyncButtonRight->getToggleState() == 1)
    {
        m_delayMsSliderRight->setVisible(false);
        addAndMakeVisible(m_delayBpmSliderRight.get());
    }
    else
    {
        m_delayBpmSliderRight->setVisible(false);
        addAndMakeVisible(m_delayMsSliderRight.get());
    }
}

