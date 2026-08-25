#include "Reverb.h"


// =================================================================================================
DelayLine::DelayLine(juce::AudioProcessor& p, juce::String name) : audioProcessor(p),
                                                                   name(name)
{
	DelayLine::m_sampleRate = static_cast<int>(p.getSampleRate());
    DelayLine::m_delayBuffer.setSize(p.getTotalNumInputChannels(), static_cast<int>(DelayLine::m_sampleRate * MAX_DELAYLINE_TIME));

    DBG("new DelayLine(): " << DelayLine::name);
}


DelayLine::~DelayLine()
{
}



// =================================================================================================
void DelayLine::addToDelayLine(juce::AudioBuffer<float>& buffer)
{
	int bufferSize = buffer.getNumSamples();
	int delayBufferSize = m_delayBuffer.getNumSamples();

    for (int i = 0; i < buffer.getNumChannels(); ++i)
    {
        if (delayBufferSize >= bufferSize + m_writePosition)
        {
            m_delayBuffer.copyFrom(i, m_writePosition, buffer.getReadPointer(i), bufferSize);
        }
        else
        {
            int numSamplesToEnd = delayBufferSize - m_writePosition;
            int numSamplesAtStart = bufferSize - numSamplesToEnd;

            m_delayBuffer.copyFrom(i, m_writePosition, buffer.getReadPointer(i), numSamplesToEnd);
            m_delayBuffer.copyFrom(i, 0, buffer.getReadPointer(i, numSamplesToEnd), numSamplesAtStart);
        }
    }

    DelayLine::updateWritePosition(buffer);
}


void DelayLine::updateWritePosition(juce::AudioBuffer<float>& buffer)
{
    m_writePosition += buffer.getNumSamples();
    m_writePosition %= m_delayBuffer.getNumSamples();
}


void DelayLine::addToBuffer(juce::AudioBuffer<float>& buffer, int srcChannel, int destChannel)
{
    int delayBufferSize = m_delayBuffer.getNumSamples();
    int bufferSize = buffer.getNumSamples();

    float const_gain = 1.0f;
    int delaySamples = static_cast<int>(m_delayTime * m_sampleRate);
    int readPosition = m_writePosition - delaySamples;


    if (readPosition < 0)
    {
        readPosition = delayBufferSize + readPosition;
    }

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(destChannel, 0, m_delayBuffer.getReadPointer(srcChannel, readPosition), bufferSize, const_gain, const_gain);
    }
    else
    {
        int numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(destChannel, 0, m_delayBuffer.getReadPointer(srcChannel, readPosition), numSamplesToEnd, const_gain, const_gain);

        int numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(destChannel, numSamplesToEnd, m_delayBuffer.getReadPointer(srcChannel, 0), numSamplesAtStart, const_gain, const_gain);
    }
}

void DelayLine::copyToBuffer(juce::AudioBuffer<float>& buffer, int srcChannel, int destChannel)
{
    int delayBufferSize = m_delayBuffer.getNumSamples();
    int bufferSize = buffer.getNumSamples();

    float const_gain = 1.0f;
    int delaySamples = static_cast<int>(m_delayTime * m_sampleRate);
    int readPosition = m_writePosition - delaySamples;


    if (readPosition < 0)
    {
        readPosition = delayBufferSize + readPosition;
    }

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.copyFromWithRamp(destChannel, 0, m_delayBuffer.getReadPointer(srcChannel, readPosition), bufferSize, const_gain, const_gain);
    }
    else
    {
        int numSamplesToEnd = delayBufferSize - readPosition;
        buffer.copyFromWithRamp(destChannel, 0, m_delayBuffer.getReadPointer(srcChannel, readPosition), numSamplesToEnd, const_gain, const_gain);

        int numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.copyFromWithRamp(destChannel, numSamplesToEnd, m_delayBuffer.getReadPointer(srcChannel, 0), numSamplesAtStart, const_gain, const_gain);
    }
}



// =======================================================================================
ReverbStage::ReverbStage(juce::AudioProcessor& p, juce::String name, 
                         juce::AudioProcessorValueTreeState& params) : audioProcessor(p),
                                                                       name(name),
                                                                       m_params(params)
{
    createDelayLines(p);
}


ReverbStage::~ReverbStage()
{
    std::vector<std::shared_ptr<DelayLine>>::iterator itr;

    for (itr = m_delayLines.begin(); itr != m_delayLines.end(); ++itr)
    {
        (*itr) = nullptr;
    }
}


juce::Array<float> ReverbStage::createMixMatrix()
{
    std::array<float, N_CH_REV_BUFF* N_CH_REV_BUFF> mixMat = { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                                                              1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1,  1, -1,
                                                              1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,
                                                              1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1,  1, -1, -1,  1,
                                                              1,  1,  1,  1, -1, -1, -1, -1,  1,  1,  1,  1, -1, -1, -1, -1,
                                                              1, -1,  1, -1, -1,  1, -1,  1,  1, -1,  1, -1, -1,  1, -1,  1,
                                                              1,  1, -1, -1, -1, -1,  1,  1,  1,  1, -1, -1, -1, -1,  1,  1,
                                                              1, -1, -1,  1, -1,  1,  1, -1,  1, -1, -1,  1, -1,  1,  1, -1,
                                                              1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                              1, -1,  1, -1,  1, -1,  1, -1, -1,  1, -1,  1, -1,  1, -1,  1,
                                                              1,  1, -1, -1,  1,  1, -1, -1, -1, -1,  1,  1, -1, -1,  1,  1,
                                                              1, -1, -1,  1,  1, -1, -1,  1, -1,  1,  1, -1, -1,  1,  1, -1,
                                                              1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  1,  1,  1,
                                                              1, -1,  1, -1, -1,  1, -1,  1, -1,  1, -1,  1,  1, -1,  1, -1,
                                                              1,  1, -1, -1, -1, -1,  1,  1, -1, -1,  1,  1,  1,  1, -1, -1,
                                                              1, -1, -1,  1, -1,  1,  1, -1, -1,  1,  1, -1,  1, -1, -1,  1 };

    juce::Array<float> temp_mixMat;
    temp_mixMat.resize(N_CH_REV_BUFF * N_CH_REV_BUFF);
    memcpy(temp_mixMat.getRawDataPointer(), mixMat.data(), mixMat.size());
    return temp_mixMat;
}

std::shared_ptr<DelayLine> ReverbStage::getLine(juce::String name)
{
    for (int i = 0; i < ReverbStage::m_delayLines.size(); ++i)
    {
        if (ReverbStage::m_delayLines[i]->name == name)
        {
            return ReverbStage::m_delayLines[i];
        }
    }

    DBG("ERROR: DelayLine() does not exist: " << name);
    jassertfalse;
}


void ReverbStage::createDelayLines(juce::AudioProcessor& p)
{
    ReverbStage::m_delayline00 = std::make_shared<DelayLine>(p, "DelayLine 0");
    ReverbStage::m_delayline01 = std::make_shared<DelayLine>(p, "DelayLine 1");
    ReverbStage::m_delayline02 = std::make_shared<DelayLine>(p, "DelayLine 2");
    ReverbStage::m_delayline03 = std::make_shared<DelayLine>(p, "DelayLine 3");
    ReverbStage::m_delayline04 = std::make_shared<DelayLine>(p, "DelayLine 4");
    ReverbStage::m_delayline05 = std::make_shared<DelayLine>(p, "DelayLine 5");
    ReverbStage::m_delayline06 = std::make_shared<DelayLine>(p, "DelayLine 6");
    ReverbStage::m_delayline07 = std::make_shared<DelayLine>(p, "DelayLine 7");

    ReverbStage::m_delayLines.reserve(NUM_DELAY_LINES);

    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline00);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline01);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline02);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline03);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline04);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline05);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline06);
    ReverbStage::m_delayLines.push_back(ReverbStage::m_delayline07);

    jassert(ReverbStage::m_delayLines.size() <= NUM_DELAY_LINES);
}


void ReverbStage::setup(juce::AudioProcessor& p, juce::AudioBuffer<float>& srcBuffer)
{
    int sampleRate = static_cast<int>(p.getSampleRate());

    for (int i = 0; i < m_delayLines.size(); ++i)
    {
        m_delayLines[i]->m_delayTime = sampleRate;
        m_delayLines[i]->m_delayBuffer.setSize(p.getTotalNumOutputChannels(), sampleRate, false, true, false);
    }

    m_revBuffer.setSize(N_CH_REV_BUFF, srcBuffer.getNumSamples(), false, true, false);
}


void ReverbStage::updateSpace(float perc)
{
    std::atomic<float>* space = m_params.getRawParameterValue("SPACE");

    ReverbStage::m_delayline00->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.00f;
    ReverbStage::m_delayline01->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.01f;
    ReverbStage::m_delayline02->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.02f;
    ReverbStage::m_delayline03->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.03f;
    ReverbStage::m_delayline04->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.04f;
    ReverbStage::m_delayline05->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.05f;
    ReverbStage::m_delayline06->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.06f;
    ReverbStage::m_delayline07->m_delayTime = MIN_DELAYLINE_TIME + space->load() * 0.07f;

    DBG("DelayLine07 delaytime: " << ReverbStage::m_delayline07->m_delayTime);

    jassert(MIN_DELAYLINE_TIME + space->load() * 0.07f < MAX_DELAYLINE_TIME);
}


void ReverbStage::fillRevBuffer(juce::AudioBuffer<float>& srcBuffer)
{
    jassert(srcBuffer.getNumSamples() == m_revBuffer.getNumSamples());

    int nInputChannels = audioProcessor.getTotalNumInputChannels();

    for (int i = 0; i < ReverbStage::m_delayLines.size(); ++i)
    {
        ReverbStage::m_delayLines[i]->addToDelayLine(srcBuffer);

        for (int channel = 0; channel < nInputChannels; ++channel)
        {
            ReverbStage::m_delayLines[i]->copyToBuffer(m_revBuffer, channel, channel + nInputChannels * i);
        }
    }
}


void ReverbStage::matMul()
{
    juce::AudioBuffer<float> temp_rev_buf;
    m_revBuffer.clear();
    temp_rev_buf.makeCopyOf(m_revBuffer);

    /* Access m_mixMat matrix elements -> index = (row * totalColumns) + column */
    for (int k = 0; k < temp_rev_buf.getNumSamples(); ++k)
    {
        for (int channel = 0; channel < N_CH_REV_BUFF; ++channel)
        {
            for (int mat = 0; mat < N_CH_REV_BUFF; ++mat)
            {
                m_revBuffer.setSample(channel, k, (m_revBuffer.getSample(channel, k) + m_mixMat[channel * N_CH_REV_BUFF + mat] * temp_rev_buf.getSample(mat, k)) / 16);
            }
        }
    }
}


void ReverbStage::sumRevBufferAndAddTo(juce::AudioBuffer<float>& buffer, float perc)
{
    int nOutputChannels = audioProcessor.getTotalNumOutputChannels();

    std::atomic<float>* decay = m_params.getRawParameterValue("DECAY");

    for (int channel = 0; channel < nOutputChannels; ++channel)
    {
        for (int i = 0; i < m_delayLines.size(); ++i)
        {
            buffer.addFromWithRamp(channel, 0, m_revBuffer.getReadPointer(channel + nOutputChannels * i), buffer.getNumSamples(),
                decay->load() * perc, decay->load() * perc);
        }
    }
}


void ReverbStage::process(juce::AudioBuffer<float>& srcBuffer, float perc)
{
    updateSpace(perc);
    fillRevBuffer(srcBuffer);
    matMul();
    sumRevBufferAndAddTo(srcBuffer, perc);
}


// =======================================================================================
Reverb::Reverb(juce::AudioProcessor& p, juce::AudioProcessorValueTreeState& params)
{
    createProcessor(p, params);
}


Reverb::~Reverb()
{
    std::vector<std::shared_ptr<ReverbStage>>::iterator itr;

    for (itr = m_reverbStages.begin(); itr != m_reverbStages.end(); ++itr)
    {
        (*itr) = nullptr;
    }
}


void Reverb::createProcessor(juce::AudioProcessor& p, juce::AudioProcessorValueTreeState& params)
{
    m_reverbStage00 = std::make_shared<ReverbStage>(p,"Stage0", params);
    m_reverbStage01 = std::make_shared<ReverbStage>(p,"Stage1", params);
    m_reverbStage02 = std::make_shared<ReverbStage>(p,"Stage2", params);
    m_reverbStage03 = std::make_shared<ReverbStage>(p,"Stage3", params);

    m_reverbStages.reserve(NUM_REV_STAGES);
    
    m_reverbStages.push_back(m_reverbStage00);
    m_reverbStages.push_back(m_reverbStage01);
    m_reverbStages.push_back(m_reverbStage02);
    m_reverbStages.push_back(m_reverbStage03);

    jassert(m_reverbStages.size() <= NUM_REV_STAGES);
}


void Reverb::setup(juce::AudioProcessor& p, juce::AudioBuffer<float>& srcBuffer)
{
    m_reverbStage00->setup(p, srcBuffer);
    m_reverbStage01->setup(p, srcBuffer);
    m_reverbStage02->setup(p, srcBuffer);
    m_reverbStage03->setup(p, srcBuffer);
}

void Reverb::process(juce::AudioBuffer<float>& srcBuffer)
{
    m_reverbStage00->process(srcBuffer, 1.00f);
    m_reverbStage01->process(srcBuffer, 0.75f);
    m_reverbStage02->process(srcBuffer, 0.50f);
    m_reverbStage03->process(srcBuffer, 0.25f);
}