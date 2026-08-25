#include "Headers.h"
#include "Constants.h"

// ==================================================================================
class DelayLine
{
public:
	explicit DelayLine(juce::AudioProcessor& p, juce::String name);
	~DelayLine();
	
	void DelayLine::addToDelayLine(juce::AudioBuffer<float>& buffer);
	void DelayLine::addToBuffer(juce::AudioBuffer<float>& buffer, int srcChannel, int destChannel);
	void DelayLine::copyToBuffer(juce::AudioBuffer<float>& buffer, int srcChannel, int destChannel);
	
	juce::String name;
	juce::AudioBuffer<float> m_delayBuffer;	
	int m_sampleRate;
	float m_delayTime{ 0 };

private:
	void DelayLine::updateWritePosition(juce::AudioBuffer<float>& buffer);
	
	juce::AudioProcessor& audioProcessor;

	int m_writePosition{ 0 };

	JUCE_LEAK_DETECTOR(DelayLine)
};



// ==================================================================================
class ReverbStage
{
public:
	ReverbStage(juce::AudioProcessor& p, juce::String name, juce::AudioProcessorValueTreeState& params);
	~ReverbStage();

	std::shared_ptr<DelayLine> ReverbStage::getLine(juce::String name);
	void ReverbStage::setup(juce::AudioProcessor& p, juce::AudioBuffer<float>& srcBuffer);
	void ReverbStage::process(juce::AudioBuffer<float>& srcBuffer, float perc);

	std::vector<std::shared_ptr<DelayLine>> m_delayLines;
	juce::String name;

private:
	void ReverbStage::createDelayLines(juce::AudioProcessor& p);
	static juce::Array<float> ReverbStage::createMixMatrix();
	void ReverbStage::fillRevBuffer(juce::AudioBuffer<float>& buffer);
	void ReverbStage::updateSpace(float perc);
	void ReverbStage::matMul();
	void ReverbStage::sumRevBufferAndAddTo(juce::AudioBuffer<float>& buffer, float perc);

	juce::AudioProcessor& audioProcessor;
	juce::AudioProcessorValueTreeState& m_params;
	juce::AudioBuffer<float> m_revBuffer;

	std::shared_ptr<DelayLine> m_delayline00;
	std::shared_ptr<DelayLine> m_delayline01;
	std::shared_ptr<DelayLine> m_delayline02;
	std::shared_ptr<DelayLine> m_delayline03;
	std::shared_ptr<DelayLine> m_delayline04;
	std::shared_ptr<DelayLine> m_delayline05;
	std::shared_ptr<DelayLine> m_delayline06;
	std::shared_ptr<DelayLine> m_delayline07;

	const juce::Array<float> m_mixMat = createMixMatrix();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbStage);
};


class Reverb
{
public:
	Reverb(juce::AudioProcessor& p, juce::AudioProcessorValueTreeState& params);
	~Reverb();

	void Reverb::setup(juce::AudioProcessor& p, juce::AudioBuffer<float>& srcBuffer);
	void Reverb::process(juce::AudioBuffer<float>& srcBuffer);

private:
	void Reverb::createProcessor(juce::AudioProcessor& p, juce::AudioProcessorValueTreeState& params);

	std::shared_ptr<ReverbStage> m_reverbStage00;
	std::shared_ptr<ReverbStage> m_reverbStage01;
	std::shared_ptr<ReverbStage> m_reverbStage02;
	std::shared_ptr<ReverbStage> m_reverbStage03;

	std::vector<std::shared_ptr<ReverbStage>> m_reverbStages;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb)
};