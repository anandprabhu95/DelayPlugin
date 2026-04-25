import numpy as np
import random
import math
import matplotlib.pyplot as plt
import scipy



gBuffLen = 512
gChannels = 2
gBuff = np.zeros((gChannels, gBuffLen), np.float32)
gSecsOfAudio = 10
PI = 3.1416

#=============================================================================================================   
class ProcessBlockContext:
    pass


class DelayLine:
    def __init__(self, delayTime: float, sampleRate: int):
        self.delaySamples = int(delayTime*sampleRate)
        self.delayBufferSize = 5 * sampleRate
        self.delayBuffer = np.ndarray((gChannels, self.delayBufferSize), np.float32)
        self.readPosition = 0
        self.writePosition = self.delaySamples
    
    def __updateWritePosition__(self):        
            self.writePosition = self.writePosition + self.buffSize
            self.writePosition = self.writePosition % self.delayBufferSize
            
    def delay(self, buffer: np.ndarray):
        self.buffSize = len(buffer[0,:])
        self.__updateWritePosition__()       
        for channel in range(0, gChannels):
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
    
    
def stageBuffer(aud: np.ndarray, readPos: int, channel: int):
    global gBuff   
    for i in range(0,gBuffLen):
        gBuff[channel,i] = aud[channel,readPos+i]
        assert(len(gBuff[channel,:]) == gBuffLen)
    

def writeOutput(buff: np.ndarray, writePos: int, channel: int):
    global output
    for i in range(0,len(buff[channel,:])):
        output[channel, writePos+i] = buff[channel, i]
        
    
def processBuffer(buff: np.ndarray, context: ProcessBlockContext):
    context.delayLine.delay(buff)
    return 0
    
                       
#=============================================================================================================
wavData = loadWav(r"untitled.wav")
aud = wavData[1]
sampleRate = wavData[0]
start = 0
output = np.zeros((gChannels, len(aud[0,:])), np.float32)
samples = len(aud[0,:])

c = ProcessBlockContext()
c.delayLine = DelayLine(1,sampleRate)

while(samples > 0):
    if (start > len(aud[0,:])-gBuffLen):
        print("Finished")
        break
    
    for channel in range(0, gChannels):
        stageBuffer(aud, start, channel)

    processBuffer(gBuff, c)
    
    for channel in range(0, gChannels):
        writeOutput(gBuff, start, channel)
    
    start = start + gBuffLen
    samples = samples - gBuffLen


plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='Input')
plt.plot(range(0,len(aud[0,:])),output[0,:],'--',label='Output')
plt.plot(range(0,len(c.delayLine.delayBuffer[0,:])),c.delayLine.delayBuffer[0,:],'--',label='DelayLine')
plt.legend(loc='upper right')
plt.show()


    

    
    