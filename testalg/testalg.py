import numpy as np
import random
import math
import matplotlib.pyplot as plt
import scipy


#=============================================================================================================   
class ProcessBlockContext:
    pass

        
class DelayLine:
    def __init__(self, delayTime: float, sampleRate: int, nChannels: int):
        self.delaySamples = int(delayTime*sampleRate)
        self.delayBufferSize = 5 * sampleRate
        self.delayBuffer = np.ndarray((nChannels, self.delayBufferSize), np.float32)
        self.readPosition = 0
        self.writePosition = 0
        self.nChannels = nChannels
    
    def __updateWritePosition__(self):        
            self.writePosition = self.writePosition + self.buffSize
            self.writePosition = self.writePosition % self.delayBufferSize
            
    def delay(self, buffer: np.ndarray):
        self.buffSize = len(buffer[0,:])
            
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
                    
        self.__updateWritePosition__()
                    
    def addToBuffer(self, buffer: np.ndarray, feedbackGain: float):
        bufferSize = len(buffer[0,:])
        self.readPosition = self.writePosition - self.delaySamples
        
        if (self.readPosition < 0):
            self.readPosition = self.readPosition + self.delayBufferSize
            
        if (self.readPosition + bufferSize < self.delayBufferSize):
            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize):
                    buffer[channel,i] = buffer[channel,i] + self.delayBuffer[channel,self.readPosition+i]*feedbackGain
        else:
            end = self.delayBufferSize - self.readPosition 
            for channel in range(0, self.nChannels):
                for i in range(0,end):
                    buffer[channel,i] = buffer[channel,i] + self.delayBuffer[channel,self.readPosition+i]*feedbackGain

            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize-end):
                    buffer[channel,end+i] = buffer[channel,end+i] + self.delayBuffer[channel,0+i]*feedbackGain
                            


class Stream():
    def __init__(self, audioData: np.ndarray, sampleRate: int, bufferSize: int, streamTime: float):
        self.data = audioData
        self.nChannels = len(audioData[:,0])
        self.sampleRate = sampleRate
        self.bufferSize = bufferSize 
        self.readPosition = 0
        self.writePosition = 0
        self.streamTime = streamTime
        self.streamSamples = streamTime * sampleRate
        self.wavSamples = len(audioData[0,:])
        self.endOfWavData = False
        self.buffer = np.zeros((self.nChannels, bufferSize), np.float32)
        self.out = np.zeros((self.nChannels, self.streamSamples+self.bufferSize), np.float32)
    
    def startStream(self): 
        if (self.readPosition + self.bufferSize > self.streamSamples):
            print("Finished")
            return False

        if (self.readPosition + self.bufferSize < self.wavSamples):
            for channel in range(0, self.nChannels):
                for i in range(0,self.bufferSize):
                    self.buffer[channel,i] = self.data[channel,self.readPosition + i]

        else:
            if (self.endOfWavData == False):
                end = self.wavSamples - self.readPosition
                for channel in range(0, self.nChannels):
                    for i in range(0,end):
                        self.buffer[channel,i] = self.data[channel,self.readPosition + i]
                    for i in range(end,self.bufferSize):
                        self.buffer[channel,i] = 0
                
                self.endOfWavData = True
            else:
                for channel in range(0, self.nChannels):
                    for i in range(0,self.bufferSize):
                        self.buffer[channel,i] = 0
                        
        self.readPosition = self.readPosition + self.bufferSize
        return True
        
    def output(self):
        for channel in range(0, self.nChannels):
            for i in range(0,self.bufferSize):
                self.out[channel, self.writePosition+i] = self.buffer[channel,i]
        self.writePosition = self.writePosition + self.bufferSize

        

def loadWav(file: str):
    sampleRate, wav = scipy.io.wavfile.read(file)
    audioData = np.ndarray((2, len(wav)),  np.float32)
    
    if (wav.dtype == 'int16'):
        normFactor = 15
    elif(wav.dtype == 'int32'):
        normFactor = 31
    else:
        raise ValueError("Bit-depth not supported. Load a 16 or 32 bit int wav file.")
    
    for channel in range(0,2):
        for i in range(0,len(wav)):
            audioData[channel,i] = wav[i,channel]/(2**normFactor)
    
    return sampleRate, audioData
    
    
def writeWav(file: str, data: np.ndarray, sampleRate):
    amplitude = np.iinfo(np.int16).max
    writeData = amplitude * data
    print("Peaks: " + str(np.min(writeData)) + "," + str(np.max(writeData)))
    writeData = np.clip(writeData, -amplitude, amplitude).T   
    scipy.io.wavfile.write(file, sampleRate, writeData.astype(np.int16))
    
    
def processBuffer(buff: np.ndarray, context: ProcessBlockContext):
    context.delayLine1.addToBuffer(buff,0.2)
    context.delayLine1.delay(buff)       
    context.delayLine2.addToBuffer(buff,0.15030)
    context.delayLine2.delay(buff)
    context.delayLine3.addToBuffer(buff,0.0862)
    context.delayLine3.delay(buff)    
    context.delayLine4.addToBuffer(buff,0.128437)
    context.delayLine4.delay(buff)    
    context.delayLine5.addToBuffer(buff,0.175703)
    context.delayLine5.delay(buff)
    context.delayLine6.addToBuffer(buff,0.148844)
    context.delayLine6.delay(buff)    
    
    return 0
    
    
#=============================================================================================================
sampleRate, aud = loadWav(r"untitled.wav")
bufferSize = 512
SimTime = 10

stream = Stream(aud, sampleRate, bufferSize, SimTime)

c = ProcessBlockContext()
c.delayLine1 = DelayLine(0.061,sampleRate, stream.nChannels)
c.delayLine2 = DelayLine(0.04923,sampleRate, stream.nChannels)
c.delayLine3 = DelayLine(0.060925,sampleRate, stream.nChannels)
c.delayLine4 = DelayLine(0.04927,sampleRate, stream.nChannels)
c.delayLine5 = DelayLine(0.05929,sampleRate, stream.nChannels)
c.delayLine6 = DelayLine(0.05931,sampleRate, stream.nChannels)

flag = True

while(flag):    
    flag = stream.startStream()

    processBuffer(stream.buffer, c)
    
    stream.output()   


writeWav(r"output.wav", stream.out, sampleRate)

plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
#plt.plot(range(0,len(stream.data[0,:])),stream.data[0,:],'-',label='Stream')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
#plt.plot(range(0,len(c.delayLine1.delayBuffer[0,:])),c.delayLine1.delayBuffer[0,:],'--',label='DelayLine1')
#plt.plot(range(0,len(c.delayLine2.delayBuffer[0,:])),c.delayLine2.delayBuffer[0,:],'--',label='DelayLine2')
plt.legend(loc='upper right')
plt.show()
