#include "Delay/PluginProcessor.h"
#include "Delay/PluginEditor.h"

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
                        m_lowPassFilterLeft(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0f, 20000.f, 0.1f)),
                        m_lowPassFilterRight(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0f, 20000.f, 0.1f))
                        //reverb(new Reverb())
#endif
{
    params.state = juce::ValueTree("savedParams");
}

DelayAudioProcessor::~DelayAudioProcessor()
{
    //delete reverb;
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
    auto delayBufferSize = sampleRate * DELAY_BUF_SIZE; // X seconds of audio.
    m_delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize), false, true, false);
    m_wetBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock, false, true, false);

    // Ramp parameter values to target value at desired rate.
    m_feedbackGainInterpolatorLeft.reset(sampleRate, 0.01);
    m_feedbackGainInterpolatorRight.reset(sampleRate, 0.01);
    m_delayTimeInterpolatorLeft.reset(sampleRate, 0.01);
    m_delayTimeInterpolatorRight.reset(sampleRate, 0.01);
    m_drywetInterpolator.reset(sampleRate, 0.1);
    m_lfoFreqInterpolator.reset(sampleRate, 0.1);
    m_lfoAmtInterpolator.reset(sampleRate, 0.1);

    lastSampleRate = sampleRate;

    // Process spec for LowPass Filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    m_lowPassFilterLeft.prepare(spec);
    m_lowPassFilterLeft.reset();

    m_lowPassFilterRight.prepare(spec);
    m_lowPassFilterRight.reset();
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
        //reverb->reverb(buffer, getNumInputChannels());
    }
}

void DelayAudioProcessor::lowPass(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> inputBlock(buffer);
    updateFilter();
    m_lowPassFilterLeft.process(juce::dsp::ProcessContextReplacing<float>(inputBlock.getSingleChannelBlock(0)));
    m_lowPassFilterRight.process(juce::dsp::ProcessContextReplacing<float>(inputBlock.getSingleChannelBlock(1)));
}

void DelayAudioProcessor::updateFilter()
{
    std::atomic<float>* freqLeft = params.getRawParameterValue("FILTER_CUTOFF_LEFT");
    std::atomic<float>* freqRight = params.getRawParameterValue("FILTER_CUTOFF_RIGHT");
    *m_lowPassFilterLeft.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freqLeft->load());
    *m_lowPassFilterRight.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freqRight->load());
}


float DelayAudioProcessor::delayTimeFromBpmSlider(juce::String parameterID)
{
    juce::AudioPlayHead* playHead = this->getPlayHead();
    m_bpm = playHead->getPosition()->getBpm();

    std::atomic<float>* delayNoteSetg = params.getRawParameterValue(parameterID);
    float delayTime = 0.0f;

    if (m_bpm.hasValue())
    {
        float oneBeatTime = 60.0f / (*m_bpm);

        switch (static_cast<int>(delayNoteSetg->load()))
        {
        case 0:
            delayTime = oneBeatTime * (1.0f/4.0f);
            break;
        case 1:
            delayTime = oneBeatTime * (1.0f/2.0f);
            break;
        case 2:
            delayTime = oneBeatTime * (1.0f);
            break;
        case 3:
            delayTime = oneBeatTime * (2.0f);
            break;
        case 4:
            delayTime = oneBeatTime * (4.0f);
            break;
        case 5:
            delayTime = oneBeatTime * (1.5f/4.0f);
            break;
        case 6:
            delayTime = oneBeatTime * (1.5f/2.0f);
            break;
        case 7:
            delayTime = oneBeatTime * (1.5f);
            break;
        case 8:
            delayTime = oneBeatTime * (1.5f*2.0f);
            break;
        case 9:
            delayTime = oneBeatTime * (1.5f*4.0f);
            break;
        case 10:
            delayTime = oneBeatTime * (1.0f/6.0f);
            break;
        case 11:
            delayTime = oneBeatTime * (1.0f/3.0f);
            break;
        case 12:
            delayTime = oneBeatTime * (2.0f/3.0f);
            break;
        case 13:
            delayTime = oneBeatTime * (4.0f/3.0f);
            break;
        case 14:
            delayTime = oneBeatTime * (8.0f/3.0f);
            break;
        default:
            delayTime = 60.0f / 120.0f;
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

    float minDelayTime{ 0.0f }; float maxDelayTime{ MAX_DELAY_TIME }; float defaultDelayTime{ 1.0f };
    int minDelayBpmMode{ 0 }; int maxDelayBpmMode{ 14 }; int defaultDelayBpmMode{ 3 };
    float minGain{ 0.0f }; float maxGain{ 1.0f }; float defaultGain{ 0.6f };
    int minMix{ 0 }; int maxMix{ 100 }; int defaultMix{ 50 };
    float minLfoFreq{ 1.0f }; float maxLfoFreq{ 10.f }; float defaultLfoFreq{ 5 };
    float minLfoAmt{ 0.0f }; float maxLfoAmt{ 1.0f }; float defaultLfoAmt{ 0.3f };
    float minFiltCutoff{ 200.0f }; float maxFiltCutoff{ 20000.0f }; float defaultFiltCutoff{ 5000.0f }; float filterInterval{ 1.0f }; float filterSkewFactor{ 0.2f };
    bool defaultButtonStatus{ false };

    // Sliders
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DELAYMS_LEFT", "Delay Ms Left", minDelayTime, maxDelayTime, defaultDelayTime));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DELAYMS_RIGHT", "Delay Ms Right", minDelayTime, maxDelayTime, defaultDelayTime));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DELAYBPM_LEFT", "Delay BPM Left", minDelayBpmMode, maxDelayBpmMode, defaultDelayBpmMode));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DELAYBPM_RIGHT", "Delay BPM Right", minDelayBpmMode, maxDelayBpmMode, defaultDelayBpmMode));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKGAIN_LEFT", "Feedback Gain Left", minGain, maxGain, defaultGain));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKGAIN_RIGHT", "Feedback Gain Right", minGain, maxGain, defaultGain));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DRYWET", "Dry/Wet", minMix, maxMix, defaultMix));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFOFREQ", "LFO Freq", minLfoFreq, maxLfoFreq, defaultLfoFreq));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFOAMT", "LFO Amt", minLfoAmt, maxLfoAmt, defaultLfoAmt));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF_LEFT", "Filter Cutoff Left", 
                                                                      juce::NormalisableRange<float>(minFiltCutoff, maxFiltCutoff, filterInterval, filterSkewFactor), 
                                                                      defaultFiltCutoff));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_CUTOFF_RIGHT", "Filter Cutoff Right", 
                                                                      juce::NormalisableRange<float>(minFiltCutoff, maxFiltCutoff, filterInterval, filterSkewFactor), 
                                                                      defaultFiltCutoff));
    
    // Buttons
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("LFOENA", "Enable LFO", defaultButtonStatus));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("TESTRVRB", "Test Reverb", defaultButtonStatus));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("STRODEL", "Stereo Delay", defaultButtonStatus));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("BPMSYNC_LEFT", "BPM Sync Left", defaultButtonStatus));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("BPMSYNC_RIGHT", "BPM Sync Right", defaultButtonStatus));

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

    float i = 0.0f;
    for (i = lfoSinIndexStart; i < lfoSinIndexStart + deltaIndex; i+=increment)
    {
        double mSinVal = sin(static_cast<float> (i) * (2 * PI) * lfoFreq->load());
        mSinVal = lfoAmt->load() * mSinVal + (1 - lfoAmt->load());
        amplitudeVec.push_back(static_cast<float>(mSinVal));
    }
    if (i > 1/lfoFreq->load())
    {
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