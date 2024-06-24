#pragma once

#include <JuceHeader.h>
#include "Constants.h"

//==============================================================================

class DelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState params;

private:
    void DelayAudioProcessor::fillBuffer(juce::AudioBuffer<float>& wetBuffer, int channel);
    void DelayAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& wetBuffer, juce::AudioBuffer<float>& delayBuffer, int channel);
    void DelayAudioProcessor::updateWritePositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);
    void DelayAudioProcessor::mixDryWet(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& wetBuffer, int channel);
    juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameters();
    float DelayAudioProcessor::knobValRangeScaler(float paramToScale, float knobValMin, float knobValMax, float desiredSclMin, float desiredSclMax);
    std::pair<std::vector<float>, float> DelayAudioProcessor::createSinArray(juce::AudioBuffer<float>& wetBuffer, float lfoSinIndexPrevious);
    void DelayAudioProcessor::lfoAmplitudeModulation(juce::AudioBuffer<float>& wetBuffer, int channel, std::vector<float> amplitudeVec);


    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> lfoBuffer;
    int writePosition { 0 };
    int readPosition { 0 };
    juce::LinearSmoothedValue<float> feedbackGainInterpolator { 0.0f };
    juce::LinearSmoothedValue<float> delayTimeInterpolator { 0.0f };
    juce::LinearSmoothedValue<float> drywetInterpolator { 0.0f };

    float lfoSinIndexPrev {0.0f};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};
