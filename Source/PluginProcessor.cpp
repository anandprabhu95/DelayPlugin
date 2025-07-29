#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), 
                        params (*this, nullptr, "Parameters", createParameters()),
                        lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0f, 20000.f, 0.1f)),
                        reverb(new Reverb())
#endif
{
    params.state = juce::ValueTree("savedParams");
}

DelayAudioProcessor::~DelayAudioProcessor()
{
    delete reverb;
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * MAX_DELAY_TIME; // 2 seconds of audio.
    m_delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));
    m_wetBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    // Ramp parameter values to target value at desired rate.
    m_feedbackGainInterpolatorLeft.reset(sampleRate, 0.0001);
    m_feedbackGainInterpolatorRight.reset(sampleRate, 0.0001);
    m_delayTimeInterpolatorLeft.reset(sampleRate, 0.00005);
    m_delayTimeInterpolatorRight.reset(sampleRate, 0.00005);
    m_drywetInterpolator.reset(sampleRate, 0.005);
    m_lfoFreqInterpolator.reset(sampleRate, 0.0005);
    m_lfoAmtInterpolator.reset(sampleRate, 0.0005);

    lastSampleRate = sampleRate;

    // Process spec for LowPass Filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels  = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    int bufferSize = buffer.getNumSamples();
    int delayBufferSize = m_delayBuffer.getNumSamples();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
        m_wetBuffer.clear(i, 0, m_wetBuffer.getNumSamples());
    }

    // Create SinArray
    std::pair<std::vector<float>, float> sinArrayResult = createSinArray(m_wetBuffer, m_lfoSinIndexPrev);
    std::vector<float> amplVec = sinArrayResult.first;
    m_lfoSinIndexPrev = sinArrayResult.second;
    //DBG("Prev :" << lfoSinIndexPrev);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        m_wetBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel), bufferSize);

        // Copy main buffer to the delay buffer.
        fillBuffer(m_wetBuffer, channel);

        // Read from the past in delay buffer and add it to main buffer 
        readFromBuffer(m_wetBuffer, m_delayBuffer, channel);
    }

    // Low-pass the feedback loop.
    lowPass(m_wetBuffer);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Feedback the main buffer containing the contents of the delay buffer [ y(k) = x(k) + G*x(k-1) ]
        fillBuffer(m_wetBuffer, channel);     
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        lfoAmplitudeModulation(m_wetBuffer, channel, amplVec);
        
        // Dry/Wet mix
        mixDryWet(buffer, m_wetBuffer, channel);
    }


    // Loop the write position from 0 to delay buffer size.
    updateWritePositions(buffer, m_delayBuffer);



    std::atomic<float>* isTestRvrbOn = params.getRawParameterValue("TESTRVRB");
    if (isTestRvrbOn->load())
    {
        reverb->reverb(buffer, getNumInputChannels());
    }
}

void DelayAudioProcessor::lowPass(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> inputBlock(buffer);
    updateFilter();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
}

void DelayAudioProcessor::updateFilter()
{
    std::atomic<float>* freq = params.getRawParameterValue("FILTER_CUTOFF_LEFT");
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freq->load());
}


float DelayAudioProcessor::delayTimeFromBpmSlider(juce::String parameterID)
{
    juce::AudioPlayHead* playHead = this->getPlayHead();
    m_bpm = playHead->getPosition()->getBpm();

    std::atomic<float>* delayNoteSetg = params.getRawParameterValue(parameterID);
    float delayTime = 0.0f;

    if (m_bpm.hasValue())
    {
        DBG("BPM: " << *m_bpm);
        float oneBeatTime = 60.0f / (*m_bpm);
        DBG("One Beat Time: " << oneBeatTime);

        switch (static_cast<int>(delayNoteSetg->load()))
        {
        case 0:
            delayTime = oneBeatTime * (1.0f/16.0f);
            break;
        case 1:
            delayTime = oneBeatTime * (1.0f/8.0f);
            break;
        case 2:
            delayTime = oneBeatTime * (1.0f/4.0f);
            break;
        case 3:
            delayTime = oneBeatTime * (1.0f/2.0f);
            break;
        case 4:
            delayTime = oneBeatTime * (1.0f);
            break;
        case 5:
            delayTime = oneBeatTime * (2.0f);
            break;
        case 6:
            delayTime = oneBeatTime * (4.0f);
            break;
        }
    }
    else
    {
        delayTime = 60.0f / 120.0f; // Default 120 BPM
    }
    return delayTime;
}

void DelayAudioProcessor::fillBuffer(juce::AudioBuffer<float>& wetBuffer, int channel)
{   
    int bufferSize = wetBuffer.getNumSamples();
    int delayBufferSize = m_delayBuffer.getNumSamples();

    // Check if main buffer can be copied to delay buffer without wrapping around
    if (delayBufferSize >= bufferSize + m_writePosition)
    {
        // Copy main buffer to delay buffer.
        m_delayBuffer.copyFrom(channel, m_writePosition, wetBuffer.getReadPointer(channel), bufferSize);
    }
    else
    {
        // Check how much space is left in the delay buffer.
        int numSamplesToEnd = delayBufferSize - m_writePosition;
        int numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy the samples to the end
        m_delayBuffer.copyFrom(channel, m_writePosition, wetBuffer.getReadPointer(channel), numSamplesToEnd);

        // Copy the rest from the start position of the buffer.
        m_delayBuffer.copyFrom(channel, 0, wetBuffer.getReadPointer(channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void DelayAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& wetBuffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{   
    int bufferSize = wetBuffer.getNumSamples();
    int delayBufferSize = delayBuffer.getNumSamples();

    float feedbackGain = 0.0f;
    float delayTime = 0.0f;
    float delaySamples = 0.0f;

    std::atomic<float>* isBpmSyncLeftOn = params.getRawParameterValue("BPMSYNC_LEFT");
    std::atomic<float>* isBpmSyncRightOn = params.getRawParameterValue("BPMSYNC_RIGHT");
    DBG("Is BPM SYNC right on" << isBpmSyncRightOn->load());
    std::atomic<float>* isStereoOn = params.getRawParameterValue("STRODEL");

    if (isStereoOn->load() == 1) {
        if (channel == 0)
        {
            std::atomic<float>* delayTimePointer = params.getRawParameterValue("DELAYMS_LEFT");
            m_delayTimeInterpolatorLeft.setTargetValue(delayTimePointer->load());

            if (isBpmSyncLeftOn->load() == 1)
            {
                delayTime = delayTimeFromBpmSlider("DELAYBPM_LEFT");
            }
            else
            {
                delayTime = m_delayTimeInterpolatorLeft.getNextValue();
            }
            
            delaySamples = delayTimeSecs2Samples(delayTime);
            std::atomic<float>* feedbackGainPointer = params.getRawParameterValue("FEEDBACKGAIN_LEFT");
            m_feedbackGainInterpolatorLeft.setTargetValue(feedbackGainPointer->load());
            feedbackGain = m_feedbackGainInterpolatorLeft.getNextValue();
        }
        else
        {
            std::atomic<float>* delayTime2Pointer = params.getRawParameterValue("DELAYMS_RIGHT");
            m_delayTimeInterpolatorRight.setTargetValue(delayTime2Pointer->load());
            
            if (isBpmSyncRightOn->load() == 1)
            {
                delayTime = delayTimeFromBpmSlider("DELAYBPM_RIGHT");
                DBG("DelayTime BPM: " << delayTime);
            }
            else
            {
                delayTime = m_delayTimeInterpolatorRight.getNextValue();
            }
            delaySamples = delayTimeSecs2Samples(delayTime);
    
            std::atomic<float>* feedbackGain2Pointer = params.getRawParameterValue("FEEDBACKGAIN_RIGHT");
            m_feedbackGainInterpolatorRight.setTargetValue(feedbackGain2Pointer->load());
            feedbackGain = m_feedbackGainInterpolatorRight.getNextValue();
        }
    }
    else
    {
        std::atomic<float>* delayTimePointer = params.getRawParameterValue("DELAYMS_LEFT");
        m_delayTimeInterpolatorLeft.setTargetValue(delayTimePointer->load());
        
        if (isBpmSyncLeftOn->load() == 1)
        {
            delayTime = delayTimeFromBpmSlider("DELAYBPM_LEFT");
        }
        else
        {
            delayTime = m_delayTimeInterpolatorLeft.getNextValue();
        }

        delaySamples = delayTimeSecs2Samples(delayTime);

        std::atomic<float>* feedbackGainPointer = params.getRawParameterValue("FEEDBACKGAIN_LEFT");
        m_feedbackGainInterpolatorLeft.setTargetValue(feedbackGainPointer->load());
        feedbackGain = m_feedbackGainInterpolatorLeft.getNextValue();
    }
    

    // Read "delayTime" seconds of audio in the past from the delay buffer.
    m_readPosition = m_writePosition - static_cast<int>(delaySamples);

    // Wrap around.
    if (m_readPosition < 0)
    {
        m_readPosition = delayBufferSize + m_readPosition;
    }

    if (m_readPosition + bufferSize < delayBufferSize)
    {
        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, m_readPosition), bufferSize, feedbackGain, feedbackGain);
    }
    else
    {
        int numSamplesToEnd = delayBufferSize - m_readPosition;
        wetBuffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, m_readPosition), numSamplesToEnd, feedbackGain, feedbackGain);

        int numSamplesAtStart = bufferSize - numSamplesToEnd;
        wetBuffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, feedbackGain, feedbackGain);
    }
}

void DelayAudioProcessor::updateWritePositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    // Loop the write position from 0 to delay buffer size.
    int bufferSize = buffer.getNumSamples();
    int delayBufferSize = delayBuffer.getNumSamples();
    m_writePosition += bufferSize;
    m_writePosition %= delayBufferSize;
}

float DelayAudioProcessor::delayTimeSecs2Samples(float delayTime)
{
    return delayTime * getSampleRate();
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // Sliders
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DELAYMS_LEFT", "Delay Ms Left", 0.0f, MAX_DELAY_TIME, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DELAYMS_RIGHT", "Delay Ms Right", 0.0f, MAX_DELAY_TIME, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DELAYBPM_LEFT", "Delay BPM Left", 0, 6, 3));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DELAYBPM_RIGHT", "Delay BPM Right", 0, 6, 3));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKGAIN_LEFT", "Feedback Gain Left", 0.0f, 1.0f, 0.7f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKGAIN_RIGHT", "Feedback Gain Right", 0.0f, 1.0f, 0.7f));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DRYWET", "Dry/Wet", 0, 100, 50));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFOFREQ", "LFO Freq", 1.f, 10.0f, 2.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFOAMT", "LFO Amt", 0.0f, 1.0f, 0.2f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF_LEFT", "Filter Cutoff Left", juce::NormalisableRange<float>(200.f, 20000.f, 1.0f, 0.2f), 2000.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF_RIGHT", "Filter Cutoff Right", juce::NormalisableRange<float>(200.f, 20000.f, 1.0f, 0.2f), 2000.f));
    
    // Buttons
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("LFOENA", "Enable LFO", 0));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("TESTRVRB", "Test Reverb", 0));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("STRODEL", "Stereo Delay", 0));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("BPMSYNC_LEFT", "BPM Sync Left", 0));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("BPMSYNC_RIGHT", "BPM Sync Right", 0));

    return { parameters.begin(), parameters.end() };
}

void DelayAudioProcessor::mixDryWet(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& wetBuffer, int channel)
{
    std::atomic<float>* drywetPointer = params.getRawParameterValue("DRYWET");
    m_drywetInterpolator.setTargetValue(drywetPointer->load());
    float drywetGain = m_drywetInterpolator.getNextValue();

    // Scale dry wet gain from [-1,+1] to [0,+1]
    float scaledDryWetGain = knobValRangeScaler(drywetGain, 0.0f, 100.0f, 0.0f, 1.0f);
    
    // Reduce gain on the main buffer when as the wet gain increases.
    buffer.applyGain(1.0f - 0.5f * scaledDryWetGain);
    DBG("Number of samples in main buffer: " << buffer.getNumSamples());
    buffer.addFromWithRamp(channel, 0, wetBuffer.getReadPointer(channel, 0), wetBuffer.getNumSamples(), scaledDryWetGain, scaledDryWetGain);
}

float DelayAudioProcessor::knobValRangeScaler(float paramToScale, float guiSclMin, float guiSclMax, float desiredSclMin, float desiredSclMax)
{
    float scaledParam = (desiredSclMax - desiredSclMin) * (paramToScale - guiSclMin) / (guiSclMax - guiSclMin) + desiredSclMin;
    return scaledParam;
}

//=========================================== LFO ========================================
void DelayAudioProcessor::lfoAmplitudeModulation(juce::AudioBuffer<float>& wetBuffer, int channel, std::vector<float> amplitudeVec)
{
    float drySample{ 0.0f };
    float lfoSample{ 0.0f };
    float amplitudeMod{ 0.0f };

    std::atomic<float>* isLfoOn = params.getRawParameterValue("LFOENA");

    if (isLfoOn->load() != 0) {
        for (channel = 0; channel < wetBuffer.getNumChannels(); ++channel)
        {
            for (int sample = 0; sample < wetBuffer.getNumSamples(); ++sample) {
                drySample = wetBuffer.getSample(channel, sample);
                amplitudeMod = amplitudeVec[sample];
                lfoSample = drySample * amplitudeMod;
                wetBuffer.setSample(channel, sample, lfoSample);
            }
        }
    }
}

std::pair<std::vector<float>, float> DelayAudioProcessor::createSinArray(juce::AudioBuffer<float>& wetBuffer, float lfoSinIndexPrevious) const
{   
    int wetBufferSize = wetBuffer.getNumSamples();
    std::vector<float> amplitudeVec;
    float sampleRate = static_cast<float>(getSampleRate());
    float deltaIndex = wetBufferSize / sampleRate ;
    float increment = 1.0f / sampleRate;
    float lfoSinIndexStart = lfoSinIndexPrevious;
    std::atomic<float>* lfoFreq = params.getRawParameterValue("LFOFREQ");
    std::atomic<float>* lfoAmt = params.getRawParameterValue("LFOAMT");
    //DBG("LfoFreq: " << lfoFreq->load());

    float i = 0.0f;
    for (i = lfoSinIndexStart; i < lfoSinIndexStart + deltaIndex; i+=increment)
    {
        double mSinVal = sin(static_cast<float> (i) * (2 * PI) * lfoFreq->load());
        mSinVal = lfoAmt->load() * mSinVal + (1 - lfoAmt->load());
        amplitudeVec.push_back(static_cast<float>(mSinVal));
    }
    if (i > 1/lfoFreq->load())
    {
        //DBG("Reset lfoIndex");
        i = 0.0f;
    }
    return std::pair<std::vector<float>, float>(amplitudeVec, i);
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(params.state.createXml());
    copyXmlToBinary(*xml.get(), destData);
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> savedParameters(getXmlFromBinary(data, sizeInBytes));

    if (savedParameters.get() != nullptr)
    {
        if (savedParameters->hasTagName(params.state.getType()))
        {
            params.state = juce::ValueTree::fromXml(*savedParameters.get());
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}