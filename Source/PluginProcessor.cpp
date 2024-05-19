/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
                        params (*this, nullptr, "Parameters", createParameters())
#endif
{
}

DelayAudioProcessor::~DelayAudioProcessor()
{
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
    auto delayBufferSize = sampleRate * 2.0; // 2 seconds of audio.
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));

    // Ramp parameter values to target value at desired rate.
    feedbackGainInterpolator.reset(sampleRate, 0.0005);
    delayTimeInterpolator.reset(sampleRate, 0.0001);
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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Copy main buffer to the delay buffer.
        fillBuffer(buffer, channel);

        // Read from the past in delay buffer and add it to main buffer 
        readFromBuffer(buffer, delayBuffer, channel);
        
        // Feedback the main buffer containing the contents of the delay buffer [ y(k) = x(k) + G*x(k-1) ]
        fillBuffer(buffer, channel);
    }

    // Loop the write position from 0 to delay buffer size.
    updateWritePositions(buffer, delayBuffer);

    // For debugging, remove later
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    DBG("Buffer Size: " << bufferSize);
    DBG("Delay Buffer Size: " << delayBufferSize);
    DBG("Write Buffer Size: " << writePosition);
}

void DelayAudioProcessor::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{   
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    // Check if main buffer can be copied to delay buffer without wrapping around
    if (delayBufferSize >= bufferSize + writePosition)
    {
        // Copy main buffer to delay buffer.
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), bufferSize);
    }
    else
    {
        // Check how much space is left in the delay buffer.
        auto numSamplesToEnd = delayBufferSize - writePosition;
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy the samples to the end
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), numSamplesToEnd);

        // Copy the rest from the start position of the buffer.
        delayBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void DelayAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{   
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    auto* delayTimePointer = params.getRawParameterValue("DELAYMS");
    delayTimeInterpolator.setTargetValue(delayTimePointer->load());
    float delayTime = delayTimeInterpolator.getNextValue();

    auto* feedbackGainPointer = params.getRawParameterValue("FEEDBACKGAIN");
    feedbackGainInterpolator.setTargetValue(feedbackGainPointer->load());
    float feedbackGain = feedbackGainInterpolator.getNextValue();

    // Read "delayTime" seconds of audio in the past from the delay buffer.
    auto readPosition = writePosition - static_cast<int>(delayTime);

    // Wrap around.
    if (readPosition < 0)
    {
        readPosition = delayBufferSize + readPosition;
    }

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, feedbackGain, feedbackGain);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, feedbackGain, feedbackGain);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, feedbackGain, feedbackGain);
    }
}

void DelayAudioProcessor::updateWritePositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    // Loop the write position from 0 to delay buffer size.
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}


//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor (*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DELAYMS", "Delay Ms", 0.0f, 96000.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACKGAIN", "Feedback Gain", 0.0f, 1.0f, 0.0f));
    return { parameters.begin(), parameters.end() };
}