import numpy as np
import random
import math
import matplotlib.pyplot as plt
import scipy



gBuffLen = 512
gChannels = 2
gBuff = np.zeros((gChannels, gBuffLen), np.float32)
gSecsOfAudio = 10
gSimTime = 5
PI = 3.1416

#=============================================================================================================   
class ProcessBlockContext:
    pass

        
class DelayLine:
    def __init__(self, delayTime: float, sampleRate: int, nChannels: int):
        self.delaySamples = int(delayTime*sampleRate)
        self.delayBufferSize = 5 * sampleRate
        self.delayBuffer = np.ndarray((gChannels, self.delayBufferSize), np.float32)
        self.readPosition = 0
        self.writePosition = self.delaySamples
        self.nChannels = nChannels
    
    def __updateWritePosition__(self):        
            self.writePosition = self.writePosition + self.buffSize
            self.writePosition = self.writePosition % self.delayBufferSize
            
    def delay(self, buffer: np.ndarray):
        self.buffSize = len(buffer[0,:])
        self.__updateWritePosition__()       
        for channel in range(0, self.nChannels):
            if(self.writePosition + self.buffSize < self.delayBufferSize):
                for i in range(0,self.buffSize):
                    self.delayBuffer[channel, self.writePosition+i] = buffer[channel, i]
            else:
                tempPos = self.writePosition
                end = self.delayBufferSize - self.writePosition
                for i in range(0,end):                                        
                    self.delayBuffer[channel, tempPos] = buffer[channel, i]
                    tempPos = tempPos + 1
                
                tempPos = 0
                start = end                
                for i in range(start,self.buffSize):                                        
                    self.delayBuffer[channel, tempPos] = buffer[channel, i]
                    tempPos = tempPos + 1
                    
    def addToMainBuffer(self, feedbackGain):
        global gBuff
        self.readPosition = self.writePosition - self.delaySamples
        
        if (self.readPosition < 0):
            self.readPosition = self.readPosition + self.delayBufferSize
            
        if (self.readPosition + gBuffLen < self.delayBufferSize):
            for channel in range(0, self.nChannels):
                for i in range(0,gBuffLen):
                    gBuff[channel,i] = gBuff[channel,i] + self.delayBuffer[channel,self.readPosition+i]*feedbackGain
        else:
            end = self.delayBufferSize - self.readPosition 
            for channel in range(0, self.nChannels):
                for i in range(0,end):
                    gBuff[channel,i] = gBuff[channel,i] + self.delayBuffer[channel,self.readPosition+i]*feedbackGain

            for channel in range(0, self.nChannels):
                for i in range(0,gBuffLen-end):
                    print(end+i)
                    print(len(self.delayBuffer[channel,:]))
                    gBuff[channel,end+i] = gBuff[channel,end+i] + self.delayBuffer[channel,0+i]*feedbackGain


class Stream():
    def __init__(self, audioData: np.ndarray, sampleRate: int, nChannels: int, streamTime: float):
        self.data = audioData
        self.sampleRate = sampleRate
        self.nChannels = nChannels
        self.readPosition = 0
        self.writePosition = 0
        self.streamTime = streamTime
        self.streamSamples = streamTime * sampleRate
        self.wavSamples = len(audioData[0,:])
        self.endOfWavData = False
        self.out = np.zeros((nChannels, self.streamSamples+gBuffLen), np.float32)
    
    def startStream(self): 
        global gBuff
        if (self.readPosition + gBuffLen > self.streamSamples):
            print("Finished")
            return False

        if (self.readPosition + gBuffLen < self.wavSamples):
            for channel in range(0, self.nChannels):
                for i in range(0,gBuffLen):
                    gBuff[channel,i] = self.data[channel,self.readPosition + i]
                    assert(len(gBuff[channel,:]) == gBuffLen)
        else:
            if (self.endOfWavData == False):
                end = self.wavSamples - self.readPosition
                for channel in range(0, self.nChannels):
                    for i in range(0,end):
                        gBuff[channel,i] = self.data[channel,self.readPosition + i]
                    for i in range(end,gBuffLen):
                        gBuff[channel,i] = 0
                    assert(len(gBuff[channel,:]) == gBuffLen)
                
                self.endOfWavData = True
            else:
                for channel in range(0, self.nChannels):
                    for i in range(0,gBuffLen):
                        gBuff[channel,i] = 0
                        
        self.readPosition = self.readPosition + gBuffLen
        return True
        
    def output(self):
        for channel in range(0, self.nChannels):
            for i in range(0,gBuffLen):
                self.out[channel, self.writePosition+i] = gBuff[channel,i]
        self.writePosition = self.writePosition + gBuffLen
        assert(self.writePosition % gBuffLen == 0)
        
        
def sineArray(freq: float, sampleRate):
    arr = []
    k = 0
    time = []
    for i in range (0, gSecsOfAudio * gSampleRate):
        arr.append(math.sin(2 * PI * freq * k))
        k = k + 1/sampleRate
        time.append(k)
    #plt.plot(time, arr)
    #plt.show()
    return np.array(arr)


def loadWav(file: str):
    sampleRate, wav = scipy.io.wavfile.read(file)
    audioData = np.ndarray((gChannels, len(wav)),  np.float32)
    
    if (wav.dtype == 'int16'):
        normFactor = 15
    elif(wav.dtype == 'int32'):
        normFactor = 31
    else:
        raise ValueError("Bit-depth not supported. Load a 16 or 32 bit int wav file.")
    
    for channel in range(0,gChannels):
        for i in range(0,len(wav)):
            audioData[channel,i] = wav[i,channel]/(2**normFactor)

    return sampleRate, audioData
    
    
def processBuffer(buff: np.ndarray, context: ProcessBlockContext):
    context.delayLine.addToMainBuffer(0.6)
    context.delayLine.delay(buff)
    
    #context.delayLine2.delay(buff)
    #context.delayLine2.addToMainBuffer()
    return 0
    


    
#=============================================================================================================
wavData = loadWav(r"untitled.wav")
aud = wavData[1]
sampleRate = wavData[0]
start = 0
samples = len(aud[0,:])

stream = Stream(aud, sampleRate, gChannels, gSimTime)

c = ProcessBlockContext()
c.delayLine = DelayLine(0.5,sampleRate, gChannels)
#c.delayLine2 = DelayLine(2,sampleRate, gChannels)
flag = True

while(flag):    
    flag = stream.startStream()

    processBuffer(gBuff, c)
    
    stream.output()   


plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
#plt.plot(range(0,len(stream.data[0,:])),stream.data[0,:],'-',label='Stream')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
#plt.plot(range(0,len(c.delayLine.delayBuffer[0,:])),c.delayLine.delayBuffer[0,:],'--',label='DelayLine')
#plt.plot(range(0,len(c.delayLine2.delayBuffer[0,:])),c.delayLine2.delayBuffer[0,:],'--',label='DelayLine2')
plt.legend(loc='upper right')
plt.show()
