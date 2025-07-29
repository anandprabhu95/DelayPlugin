#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Reverb.h"

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
    Reverb* reverb;

private:
    void DelayAudioProcessor::fillBuffer(juce::AudioBuffer<float>& wetBuffer, int channel);
    void DelayAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& wetBuffer, juce::AudioBuffer<float>& delayBuffer, int channel);
    float DelayAudioProcessor::delayTimeSecs2Samples(float delayTime);
    void DelayAudioProcessor::updateWritePositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);
    void DelayAudioProcessor::mixDryWet(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& wetBuffer, int channel);
    juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameters();
    float DelayAudioProcessor::knobValRangeScaler(float paramToScale, float knobValMin, float knobValMax, float desiredSclMin, float desiredSclMax);
    std::pair<std::vector<float>, float> DelayAudioProcessor::createSinArray(juce::AudioBuffer<float>& wetBuffer, float lfoSinIndexPrevious) const;
    void DelayAudioProcessor::lfoAmplitudeModulation(juce::AudioBuffer<float>& wetBuffer, int channel, std::vector<float> amplitudeVec);
    float DelayAudioProcessor::delayTimeFromBpmSlider(juce::String parameterID);
    void DelayAudioProcessor::lowPass(juce::AudioBuffer<float>& buffer);
    void DelayAudioProcessor::updateFilter();
    

    juce::AudioBuffer<float> m_wetBuffer;
    juce::AudioBuffer<float> m_delayBuffer;

    int m_writePosition { 0 };
    int m_readPosition { 0 };
    juce::LinearSmoothedValue<float> m_feedbackGainInterpolatorLeft { 0.0f };
    juce::LinearSmoothedValue<float> m_feedbackGainInterpolatorRight{ 0.0f };
    juce::LinearSmoothedValue<float> m_delayTimeInterpolatorLeft { 0.0f };
    juce::LinearSmoothedValue<float> m_delayTimeInterpolatorRight{ 0.0f };
    juce::LinearSmoothedValue<float> m_drywetInterpolator { 0.0f };
    juce::LinearSmoothedValue<float> m_lfoFreqInterpolator{ 0.0f };
    juce::LinearSmoothedValue<float> m_lfoAmtInterpolator{ 0.0f };

    float m_lfoSinIndexPrev {0.0f};

    juce::Optional<double> m_bpm;

    float lastSampleRate;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};
