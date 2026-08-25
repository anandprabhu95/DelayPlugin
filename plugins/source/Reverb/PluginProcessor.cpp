
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessor::ReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), 
                        params(*this, nullptr, "Parameters", createParameters()),
                        m_reverbProcessor(std::make_unique<Reverb>(*this, params))
#endif
{
    DBG("new ReverbAudioProcessor()");
}

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

//==============================================================================
const juce::String ReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_wetBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock, false, true, false);

    m_reverbProcessor->setup(*this, m_wetBuffer);
}

void ReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels  = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
        m_wetBuffer.clear(i, 0, m_wetBuffer.getNumSamples());
    }

    for (int channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
    {
        // Copy main buffer to wet buffer. All processing will be done on wet buffer.
        m_wetBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel), buffer.getNumSamples());
    }

    m_reverbProcessor->process(m_wetBuffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        mixDryWet(buffer, m_wetBuffer, channel);
    }

}

//==============================================================================
bool ReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbAudioProcessor::createEditor()
{
    //return new ReverbAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbAudioProcessor();
}


//=============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ReverbAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    int minMix{ 0 }; int maxMix{ 100 }; int defaultMix{ 50 };
    float minSpace{ 0 }; float maxSpace{ 1 }; float defaultSpace{ 0.5 };
    float minDecay{ 0 }; float maxDecay{ 1 }; float defaultDecay{ 0.5 };

    parameters.push_back(std::make_unique<juce::AudioParameterInt>("DRYWET", "Dry/Wet", minMix, maxMix, defaultMix));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("SPACE", "Space", minSpace, maxSpace, defaultSpace));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", minDecay, maxDecay, defaultDecay));

    return { parameters.begin(), parameters.end() };
}


void ReverbAudioProcessor::mixDryWet(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& wetBuffer, int channel)
{
    std::atomic<float>* drywetPointer = params.getRawParameterValue("DRYWET");
    m_drywetInterpolator.setTargetValue(drywetPointer->load());
    float drywetGain = m_drywetInterpolator.getNextValue();

    // Scale dry wet gain from [-1,+1] to [0,+1]
    float scaledDryWetGain = scaleValues(drywetGain, 0.0f, 100.0f, 0.0f, 1.0f);

    // Reduce gain on the main buffer when as the wet gain increases.
    buffer.applyGain(1.0f - 0.5f * scaledDryWetGain);
    buffer.addFromWithRamp(channel, 0, wetBuffer.getReadPointer(channel, 0), wetBuffer.getNumSamples(), scaledDryWetGain, scaledDryWetGain);
}

float ReverbAudioProcessor::scaleValues(float paramToScale, float guiSclMin, float guiSclMax, float desiredSclMin, float desiredSclMax)
{
    float scaledParam = (desiredSclMax - desiredSclMin) * (paramToScale - guiSclMin) / (guiSclMax - guiSclMin) + desiredSclMin;
    return scaledParam;
}