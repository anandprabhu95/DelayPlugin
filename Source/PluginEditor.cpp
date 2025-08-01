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
    hideDelayMsSliderIfBpmSync();
    setValueLabel(m_delayMsSliderValDispLeft, m_delayMsSliderLeft, "s");
    setValueLabel(m_delayMsSliderValDispRight, m_delayMsSliderRight, "s");
    setValueLabel(m_delayBpmSliderValDispLeft, m_delayBpmSliderLeft, "");
    setValueLabel(m_delayBpmSliderValDispRight, m_delayBpmSliderRight, "");
    setValueLabel(m_gainSliderValDispLeft, m_gainSliderLeft, "");
    setValueLabel(m_gainSliderValDispRight, m_gainSliderRight, "");
    setValueLabel(m_drywetSliderValDisp, m_drywetSlider, "%");
    setValueLabel(m_lfoFreqSliderValDisp, m_lfoFreqSlider, "");
    setValueLabel(m_toneSliderValDispLeft, m_filtCutoffSliderLeft, "Hz");
    setValueLabel(m_toneSliderValDispRight, m_filtCutoffSliderRight, "Hz");
}

void DelayAudioProcessorEditor::resized()
{
    resizeGUI();
}

//==============================================================================
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

void DelayAudioProcessorEditor::createGUI()
{   
    DBG("Creating GUI");
    // SLIDERS 
    createSlider(m_gainSliderLeft, juce::Slider::Rotary, "gainSliderLeft", "FEEDBACKGAIN_LEFT");
    createLabel(m_gainLabelLeft, "gainlabelleft", "Gain L", juce::Justification::centred);
    createSlider(m_gainSliderRight, juce::Slider::Rotary, "gainSliderRight", "FEEDBACKGAIN_RIGHT");
    createLabel(m_gainLabelRight, "gainlabelright", "Gain R", juce::Justification::centred);

    createSlider(m_delayMsSliderLeft, juce::Slider::Rotary, "delayMsSliderLeft", "DELAYMS_LEFT");
    createSlider(m_delayMsSliderRight, juce::Slider::Rotary, "delayMsSliderRight", "DELAYMS_RIGHT");
    createSlider(m_delayBpmSliderLeft, juce::Slider::Rotary, "delayBpmSliderLeft", "DELAYBPM_LEFT");
    createSlider(m_delayBpmSliderRight, juce::Slider::Rotary, "delayBpmSliderRight", "DELAYBPM_RIGHT");
    createLabel(m_delayMsLabelLeft, "delaylabelleft", "Time L", juce::Justification::centred);
    createLabel(m_delayMsLabelRight, "delaylabelright", "Time R", juce::Justification::centred);

    createSlider(m_drywetSlider, juce::Slider::Rotary, "drywetSlider", "DRYWET");
    createLabel(m_drywetLabel, "drywetlabel", "Mix", juce::Justification::centred);

    createSlider(m_lfoFreqSlider, juce::Slider::Rotary, "lfoFreqSlider", "LFOFREQ");
    createLabel(m_lfoFreqLabel, "lfofreqlabel", "LFO Freq", juce::Justification::centred);

    createSlider(m_lfoAmtSlider, juce::Slider::LinearHorizontal, "lfoAmtSlider", "LFOAMT");
    createLabel(m_lfoAmtLabel, "lfoamtlabel", "AMT", juce::Justification::centred);

    createSlider(m_filtCutoffSliderLeft, juce::Slider::Rotary, "filterCutoffSliderLeft", "FILTER_CUTOFF_LEFT");
    createLabel(m_toneLabelLeft, "tonelabelleft", "Tone L", juce::Justification::centred);

    createSlider(m_filtCutoffSliderRight, juce::Slider::Rotary, "filterCutoffSliderRight", "FILTER_CUTOFF_RIGHT");
    createLabel(m_toneLabelRight, "tonelabelright", "Tone R", juce::Justification::centred);

    // BUTTONS 

    createToggleButton(m_bpmSyncButtonLeft, "bpmSyncButtonLeft");
    createLabel(m_bpmSyncButtonLabelLeft, "bpmsyncbuttonlabelleft", "BPM", juce::Justification::left);

    createToggleButton(m_bpmSyncButtonRight, "bpmSyncButtonRight");
    createLabel(m_bpmSyncButtonLabelRight, "bpmsync2buttonlabelright", "BPM", juce::Justification::left);

    createToggleButton(m_stereoDelayButton, "stereoDelayButton");
    createLabel(m_stereoDelayLabel, "stereodelaybuttonlabel", "Stereo", juce::Justification::left);

    createToggleButton(m_testReverbButton, "testReverbButton");
    createLabel(m_testRvrbLabel, "testreverbbuttonlabel", "Test Reverb", juce::Justification::centred);

    createToggleButton(m_lfoButton, "lfoButton");
    createLabel(m_lfoButtonLabel, "lfobuttonlabel", "ENA", juce::Justification::centred);

    // VALUE LABELS

    initializeValueLabel(m_delayMsSliderValDispLeft);
    initializeValueLabel(m_delayMsSliderValDispRight);
    initializeValueLabel(m_delayBpmSliderValDispLeft);
    initializeValueLabel(m_delayBpmSliderValDispRight);
    initializeValueLabel(m_gainSliderValDispLeft);
    initializeValueLabel(m_gainSliderValDispRight);
    initializeValueLabel(m_drywetSliderValDisp);
    initializeValueLabel(m_lfoFreqSliderValDisp);
    initializeValueLabel(m_toneSliderValDispLeft);
    initializeValueLabel(m_toneSliderValDispRight);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::silver); // Thumb Color
    setSize(WIDTH, HEIGHT); // Plugin Window Size

    DBG("Created GUI");
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

    m_delayMsSliderValDispLeft = nullptr;
    m_delayMsSliderValDispRight = nullptr;
    m_delayBpmSliderValDispLeft = nullptr;
    m_delayBpmSliderValDispRight = nullptr;
    m_gainSliderValDispLeft = nullptr;
    m_gainSliderValDispRight = nullptr;
    m_drywetSliderValDisp = nullptr;
    m_lfoFreqSliderValDisp = nullptr;
    m_toneSliderValDispLeft = nullptr;
    m_toneSliderValDispRight = nullptr;

    m_toneLabelLeft = nullptr;
    m_filterCutoffParamAttachLeft = nullptr;
    m_filtCutoffSliderLeft = nullptr;

    m_toneLabelRight = nullptr;
    m_filterCutoffParamAttachRight = nullptr;
    m_filtCutoffSliderRight = nullptr;

    DBG("Destroyed GUI");
}

void DelayAudioProcessorEditor::resizeGUI()
{
    DBG("Resizing GUI");

    m_gainSliderLeft->setBounds(GLOBAL_X + COLUMN1_X, GLOBAL_Y + ROW1_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabelLeft->setBounds(GLOBAL_X + COLUMN1_LABEL_X, GLOBAL_Y + ROW1_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSliderLeft->setBounds(GLOBAL_X + COLUMN2_X, GLOBAL_Y + ROW1_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayBpmSliderLeft->setBounds(GLOBAL_X + COLUMN2_X, GLOBAL_Y + ROW1_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabelLeft->setBounds(GLOBAL_X + COLUMN2_LABEL_X, GLOBAL_Y + ROW1_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_gainSliderRight->setBounds(GLOBAL_X + COLUMN1_X, GLOBAL_Y + ROW2_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_gainLabelRight->setBounds(GLOBAL_X + COLUMN1_LABEL_X, GLOBAL_Y + ROW2_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_delayMsSliderRight->setBounds(GLOBAL_X + COLUMN2_X, GLOBAL_Y + ROW2_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayBpmSliderRight->setBounds(GLOBAL_X + COLUMN2_X, GLOBAL_Y + ROW2_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_delayMsLabelRight->setBounds(GLOBAL_X + COLUMN2_LABEL_X, GLOBAL_Y + ROW2_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_drywetSlider->setBounds(GLOBAL_X + COLUMN4_X, GLOBAL_Y + ROW1_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_drywetLabel->setBounds(GLOBAL_X + COLUMN4_LABEL_X, GLOBAL_Y + ROW1_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_lfoButton->setBounds(GLOBAL_X + LFOENA_BUT_X, GLOBAL_Y + LFOENA_BUT_Y, LFOENA_BUT_WIDTH, LFOENA_BUT_HEIGHT);
    m_lfoButtonLabel->setBounds(GLOBAL_X + LFOENA_LABEL_X, GLOBAL_Y + LFOENA_LABEL_Y, LFOENA_BUT_LAB_WIDTH, LFOENA_BUT_LAB_HEIGHT);

    m_lfoFreqSlider->setBounds(GLOBAL_X + COLUMN4_X, GLOBAL_Y + ROW2_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_lfoFreqLabel->setBounds(GLOBAL_X + COLUMN4_LABEL_X, GLOBAL_Y + ROW2_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_lfoAmtSlider->setBounds(GLOBAL_X + LFOAMT_SLIDER_X, GLOBAL_Y + LFOAMT_SLIDER_Y, LIN_SLIDER_WIDTH, LIN_SLIDER_HEIGHT);
    m_lfoAmtLabel->setBounds(GLOBAL_X + LFOAMT_LABEL_X, GLOBAL_Y + LFOAMT_LABEL_Y, LFOAMT_LAB_WIDTH, LFOAMT_LAB_HEIGHT);

    m_filtCutoffSliderLeft->setBounds(GLOBAL_X + COLUMN3_X, GLOBAL_Y + ROW1_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_toneLabelLeft->setBounds(GLOBAL_X + COLUMN3_LABEL_X, GLOBAL_Y + ROW1_LABEL_Y, SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_filtCutoffSliderRight->setBounds(GLOBAL_X + COLUMN3_X, GLOBAL_Y + ROW2_Y, SLIDER_WIDTH, SLIDER_HEIGHT);
    m_toneLabelRight->setBounds(GLOBAL_X + COLUMN3_LABEL_X, GLOBAL_Y + ROW2_LABEL_Y , SLIDER_LABEL_WIDTH, SLIDER_LABEL_HEIGHT);

    m_testReverbButton->setBounds(GLOBAL_X + TESTRVRB_BUT_X, GLOBAL_Y + TESTRVRB_BUT_Y, TESTRVRB_BUT_WIDTH, TESTRVRB_BUT_HEIGHT);
    m_testRvrbLabel->setBounds(GLOBAL_X + TESTRVRB_LABEL_X, GLOBAL_Y + TESTRVRB_LABEL_Y, TESTRVRB_BUT_LAB_WIDTH, TESTRVRB_BUT_LAB_HEIGHT);

    m_stereoDelayButton->setBounds(GLOBAL_X + STRODEL_BUT_X, GLOBAL_Y + STRODEL_BUT_Y, STRODEL_BUT_WIDTH, STRODEL_BUT_HEIGHT);
    m_stereoDelayLabel->setBounds(GLOBAL_X + STRODEL_LABEL_X, GLOBAL_Y + STRODEL_LABEL_Y, STRODEL_BUT_LAB_WIDTH, STRODEL_BUT_LAB_HEIGHT);

    m_bpmSyncButtonLeft->setBounds(GLOBAL_X + BPMSYNC_BUT_X, GLOBAL_Y + BPMSYNC_BUT_Y, BPMSYNC_BUT_WIDTH, BPMSYNC_BUT_HEIGHT);
    m_bpmSyncButtonLabelLeft->setBounds(GLOBAL_X + BPMSYNC_LABEL_X, GLOBAL_Y + BPMSYNC_LABEL_Y, BPMSYNC_BUT_LAB_WIDTH, BPMSYNC_BUT_LAB_HEIGHT);

    m_bpmSyncButtonRight->setBounds(GLOBAL_X + BPMSYNC2_BUT_X, GLOBAL_Y + BPMSYNC2_BUT_Y, BPMSYNC2_BUT_WIDTH, BPMSYNC2_BUT_HEIGHT);
    m_bpmSyncButtonLabelRight->setBounds(GLOBAL_X + BPMSYNC2_LABEL_X, GLOBAL_Y + BPMSYNC2_LABEL_Y, BPMSYNC2_BUT_LAB_WIDTH, BPMSYNC2_BUT_LAB_HEIGHT);

    m_delayMsSliderValDispLeft->setBounds(GLOBAL_X + COLUMN2_VALDISP_LABEL_X, GLOBAL_Y + ROW1_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_delayMsSliderValDispRight->setBounds(GLOBAL_X + COLUMN2_VALDISP_LABEL_X, GLOBAL_Y + ROW2_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_delayBpmSliderValDispLeft->setBounds(GLOBAL_X + COLUMN2_VALDISP_LABEL_X, GLOBAL_Y + ROW1_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_delayBpmSliderValDispRight->setBounds(GLOBAL_X + COLUMN2_VALDISP_LABEL_X, GLOBAL_Y + ROW2_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_gainSliderValDispLeft->setBounds(GLOBAL_X + COLUMN1_VALDISP_LABEL_X, GLOBAL_Y + ROW1_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_gainSliderValDispRight->setBounds(GLOBAL_X + COLUMN1_VALDISP_LABEL_X, GLOBAL_Y + ROW2_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_drywetSliderValDisp->setBounds(GLOBAL_X + COLUMN4_VALDISP_LABEL_X, GLOBAL_Y + ROW1_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_lfoFreqSliderValDisp->setBounds(GLOBAL_X + COLUMN4_VALDISP_LABEL_X, GLOBAL_Y + ROW2_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_toneSliderValDispLeft->setBounds(GLOBAL_X + COLUMN3_VALDISP_LABEL_X, GLOBAL_Y + ROW1_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);
    m_toneSliderValDispRight->setBounds(GLOBAL_X + COLUMN3_VALDISP_LABEL_X, GLOBAL_Y + ROW2_VALDISP_LABEL_Y, VALUE_LABEL_WIDTH, VALUE_LABEL_HEIGHT);

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
    m_filterCutoffParamAttachLeft = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FILTER_CUTOFF_LEFT", *m_filtCutoffSliderLeft);
    m_filterCutoffParamAttachRight = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "FILTER_CUTOFF_RIGHT", *m_filtCutoffSliderRight);

    DBG("Attached Params");
}

void DelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}

void DelayAudioProcessorEditor::createSlider(std::unique_ptr<juce::Slider>& slider, juce::Slider::SliderStyle sliderStyle,
                                                    juce::String componentName, juce::String propertyID)
{
    DBG("Attaching " << componentName);
    slider = std::make_unique<juce::Slider>(componentName);
    addAndMakeVisible(slider.get());
    juce::RangedAudioParameter *prop = audioProcessor.params.getParameter(propertyID);
    slider->setSkewFactor(prop->getNormalisableRange().skew);
    slider->setRange(prop->getNormalisableRange().start,prop->getNormalisableRange().end,prop->getNormalisableRange().interval);
    slider->setSliderStyle(sliderStyle);
    setTextBox(slider);
    slider->addListener(this);
}

void DelayAudioProcessorEditor::createToggleButton(std::unique_ptr<juce::ToggleButton>& button, juce::String componentName)
{
    DBG("Attaching " << componentName);
    button = std::make_unique<juce::ToggleButton>("");
    addAndMakeVisible(button.get());
    button->setSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    button->setClickingTogglesState(true);
}

void DelayAudioProcessorEditor::createLabel(std::unique_ptr<juce::Label>& label, juce::String componentName, juce::String textToDisplay, juce::Justification justification)
{
    DBG("Attaching " <<  componentName);
    label = std::make_unique<juce::Label>(componentName, textToDisplay);
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(LABEL_FONTSIZE, juce::Font::plain));
    label->setJustificationType(justification);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
}

void DelayAudioProcessorEditor::setTextBox(std::unique_ptr<juce::Slider>& slider)
{
    // Currently set to NoTextBox. The slider value is displayed as a Label component.
    juce::Colour outlinecolour = juce::Colour::fromFloatRGBA(BG_RED, BG_GREEN, BG_BLUE, BG_ALPHA);
    outlinecolour.withAlpha(0.0f);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, true, TEXTBOX_WIDTH, TEXTBOX_HEIGHT); 
    slider->setColour(juce::Slider::textBoxOutlineColourId, outlinecolour);
}

void DelayAudioProcessorEditor::initializeValueLabel(std::unique_ptr<juce::Label>& label)
{
    label = std::make_unique<juce::Label>("", "NULL");
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(VALUELABEL_FONTSIZE, juce::Font::plain));
    label->setJustificationType(juce::Justification::centred);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
}

void DelayAudioProcessorEditor::setValueLabel(std::unique_ptr<juce::Label>& label, std::unique_ptr<juce::Slider>& slider, juce::String suffix) const
{
    if (slider == m_delayBpmSliderLeft || slider == m_delayBpmSliderRight)
    {
        switch (static_cast<int>(slider->getValue()))
        {
        case 0:
            label->setText("1/64", juce::dontSendNotification);
            break;
        case 1:
            label->setText("1/32", juce::dontSendNotification);
            break;
        case 2:
            label->setText("1/16", juce::dontSendNotification);
            break;
        case 3:
            label->setText("1/8", juce::dontSendNotification);
            break;
        case 4:
            label->setText("1/4", juce::dontSendNotification);
            break;
        case 5:
            label->setText("2/4", juce::dontSendNotification);
            break;
        case 6:
            label->setText("4/4", juce::dontSendNotification);
            break;
        default:
            label->setText("Err", juce::dontSendNotification);
        }
    }
    else
    {
        label->setText(slider->getTextFromValue(slider->getValue()) + " " + suffix, juce::dontSendNotification);
    }
}

void DelayAudioProcessorEditor::componentDisable()
{
    TrackStatus stereoButtonStatus = m_StereoButtonUpdate.check(m_stereoDelayButton);
    if (stereoButtonStatus == CHANGED)
    {
        // Disable Slider2 when in mono modo.
        if (!m_stereoDelayButton->getToggleState())
        {
            m_gainSliderRight->setEnabled(false);
            m_gainSliderRight->setAlpha(0.5f);

            m_delayMsSliderRight->setEnabled(false);
            m_delayMsSliderRight->setAlpha(0.5f);

            m_delayBpmSliderRight->setEnabled(false);
            m_delayBpmSliderRight->setAlpha(0.5f);

            m_filtCutoffSliderRight->setEnabled(false);
            m_filtCutoffSliderRight->setAlpha(0.5f);

            m_bpmSyncButtonRight->setEnabled(false);
            m_bpmSyncButtonRight->setAlpha(0.5f);
            m_bpmSyncButtonLabelRight->setAlpha(0.5f);

            m_gainLabelLeft->setText("Gain", juce::dontSendNotification);
            m_gainLabelRight->setText("", juce::dontSendNotification);

            m_delayMsLabelLeft->setText("Time", juce::dontSendNotification);
            m_delayMsLabelRight->setText("", juce::dontSendNotification);

            m_toneLabelLeft->setText("Tone", juce::dontSendNotification);
            m_toneLabelRight->setText("", juce::dontSendNotification);
        }
        else
        {
            m_gainSliderRight->setEnabled(true);
            m_gainSliderRight->setAlpha(1.0f);

            m_delayMsSliderRight->setEnabled(true);
            m_delayMsSliderRight->setAlpha(1.0f);

            m_delayBpmSliderRight->setEnabled(true);
            m_delayBpmSliderRight->setAlpha(1.0f);

            m_filtCutoffSliderRight->setEnabled(true);
            m_filtCutoffSliderRight->setAlpha(1.0f);

            m_bpmSyncButtonRight->setEnabled(true);
            m_bpmSyncButtonRight->setAlpha(1.0f);
            m_bpmSyncButtonLabelRight->setAlpha(1.0f);

            m_gainLabelLeft->setText("Gain L", juce::dontSendNotification);
            m_gainLabelRight->setText("Gain R", juce::dontSendNotification);

            m_delayMsLabelLeft->setText("Time L", juce::dontSendNotification);
            m_delayMsLabelRight->setText("Time R", juce::dontSendNotification);

            m_toneLabelLeft->setText("Tone L", juce::dontSendNotification);
            m_toneLabelRight->setText("Tone R", juce::dontSendNotification);
        }
    }
    

    TrackStatus lfoButtonStatus = m_LfoButtonUpdate.check(m_lfoButton);
    if (lfoButtonStatus == CHANGED)
    {
        if (!m_lfoButton->getToggleState())
        {
            m_lfoFreqSlider->setEnabled(false);
            m_lfoFreqSlider->setAlpha(0.5f);

            m_lfoAmtSlider->setEnabled(false);
            m_lfoAmtSlider->setAlpha(0.5f);
        }
        else
        {
            m_lfoFreqSlider->setEnabled(true);
            m_lfoFreqSlider->setAlpha(1.0f);

            m_lfoAmtSlider->setEnabled(true);
            m_lfoAmtSlider->setAlpha(1.0f);
        }
    }
}

void DelayAudioProcessorEditor::hideDelayMsSliderIfBpmSync()
{
    TrackStatus bpmSyncLeftStatus = m_BpmSyncLeftButtonUpdate.check(m_bpmSyncButtonLeft);
    if (bpmSyncLeftStatus == CHANGED)
    {
        if (m_bpmSyncButtonLeft->getToggleState())
        {
            m_delayMsSliderLeft->setVisible(false);
            m_delayBpmSliderLeft->setVisible(true);
            m_delayMsSliderValDispLeft->setVisible(false);
            m_delayBpmSliderValDispLeft->setVisible(true);
        }
        else
        {
            m_delayMsSliderLeft->setVisible(true);
            m_delayBpmSliderLeft->setVisible(false);
            m_delayMsSliderValDispLeft->setVisible(true);
            m_delayBpmSliderValDispLeft->setVisible(false);
        }
    }


    TrackStatus bpmSyncRightStatus = m_BpmSyncRightButtonUpdate.check(m_bpmSyncButtonRight);
    if (bpmSyncRightStatus == CHANGED)
    {
        if (m_bpmSyncButtonRight->getToggleState())
        {
            m_delayMsSliderRight->setVisible(false);
            m_delayBpmSliderRight->setVisible(true);
            m_delayMsSliderValDispRight->setVisible(false);
            m_delayBpmSliderValDispRight->setVisible(true);
        }
        else
        {
            m_delayMsSliderRight->setVisible(true);
            m_delayBpmSliderRight->setVisible(false);
            m_delayMsSliderValDispRight->setVisible(true);
            m_delayBpmSliderValDispRight->setVisible(false);
        }
    }
}

