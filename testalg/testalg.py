import numpy as np
import random
import math
import matplotlib.pyplot as plt
import scipy


#=============================================================================================================   
class Items:
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
        self.time = 0
        self.streamSamples = int(streamTime * sampleRate)
        self.wavSamples = len(audioData[0,:])
        self.endOfWavData = False
        self.buffer = np.zeros((self.nChannels, bufferSize), np.float32)
        self.out = np.zeros((self.nChannels, self.streamSamples+self.bufferSize), np.float32)
    
    def __elapsedTime__(self):
        self.time = self.time + (self.bufferSize/self.sampleRate)
               
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
        self.__elapsedTime__()
        return True
        
    def output(self):
        for channel in range(0, self.nChannels):
            for i in range(0,self.bufferSize):
                self.out[channel, self.writePosition+i] = self.buffer[channel,i]
        self.writePosition = self.writePosition + self.bufferSize

        
class Diffuse8():
    def __init__(self, bufferSize: int):
        self.bufferSize = bufferSize
        self.nChannels = 2
        self.diffuserChannels = 8
        self.mixMatrix = np.array([[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                                   [1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1],
                                   [1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1],
                                   [1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1],
                                   [1, 1, 1, 1,-1,-1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1],
                                   [1,-1, 1,-1,-1, 1,-1, 1, 1,-1, 1,-1,-1, 1,-1, 1],
                                   [1, 1,-1,-1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1, 1],
                                   [1,-1,-1, 1,-1, 1, 1,-1, 1,-1,-1, 1,-1, 1, 1,-1],
                                   [1, 1, 1, 1, 1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1,-1],
                                   [1,-1, 1,-1, 1,-1, 1,-1,-1, 1,-1, 1,-1, 1,-1, 1],
                                   [1, 1,-1,-1, 1, 1,-1,-1,-1,-1, 1, 1,-1,-1, 1, 1],
                                   [1,-1,-1, 1, 1,-1,-1, 1,-1, 1, 1,-1,-1, 1, 1,-1],
                                   [1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 1],
                                   [1,-1, 1,-1,-1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1],
                                   [1, 1,-1,-1,-1,-1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1],
                                   [1,-1,-1, 1,-1, 1, 1,-1,-1, 1, 1,-1, 1,-1,-1, 1]])
                                   
        self.bufferMatrix = np.zeros((self.nChannels*self.diffuserChannels, self.bufferSize))
    
    def diffuse(self, d: Items):                           
        for j in range(0,self.bufferSize): 
            for i in range(0,self.diffuserChannels):
                for channel in range(0,self.nChannels):         
                    exec(f"self.bufferMatrix[2*{i}+{channel},{j}] = d.buffer{i}[{channel},{j}]")

        
        for i in range(0,self.diffuserChannels):       
            exec(f"d.buffer{str(i)} = np.zeros((self.nChannels, self.bufferSize))",locals(),globals())
        
        
        for j in range(0,self.bufferSize):       
            result = np.dot(np.array(self.bufferMatrix[:,j]), self.mixMatrix)
            for i in range(0,self.diffuserChannels):
                for channel in range(0,self.nChannels):
                    exec(f"d.buffer{str(i)}[{channel},{j}] = result[{2*i+channel}]") 
                    
                    
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
    
    
def processBuffer(buff: np.ndarray, processorItems: Items):
    
    bufferForDiffusion = Items()
    
    i = 1
    for i in range(0,8):
        exec(f"bufferForDiffusion.buffer{str(i)} = np.zeros((2, 512))",locals(),globals())
    
    print("INFO: Populating delay buffers")   
    processorItems.delayLine1.delay(buff)       
    processorItems.delayLine1.addToBuffer(bufferForDiffusion.buffer0,0.125)
    processorItems.delayLine2.delay(buff)
    processorItems.delayLine2.addToBuffer(bufferForDiffusion.buffer1,0.125)
    processorItems.delayLine3.delay(buff)    
    processorItems.delayLine3.addToBuffer(bufferForDiffusion.buffer2,0.125)
    processorItems.delayLine4.delay(buff)    
    processorItems.delayLine4.addToBuffer(bufferForDiffusion.buffer3,0.125)
    processorItems.delayLine5.delay(buff)
    processorItems.delayLine5.addToBuffer(bufferForDiffusion.buffer4,0.125)
    processorItems.delayLine6.delay(buff)
    processorItems.delayLine6.addToBuffer(bufferForDiffusion.buffer5,0.125)
    processorItems.delayLine7.delay(buff)
    processorItems.delayLine7.addToBuffer(bufferForDiffusion.buffer6,0.125)
    processorItems.delayLine8.delay(buff)
    processorItems.delayLine8.addToBuffer(bufferForDiffusion.buffer7,0.125)
    
    print("INFO: Mixing buffers")
    processorItems.diffuser.diffuse(bufferForDiffusion)
    
    for k in range(0,8):
        for channel in range(0, 2):
            for i in range(0,512):
                exec(f"buff[{channel},{i}] = buff[{channel},{i}] + bufferForDiffusion.buffer{k}[{channel},{i}]")
        
    return 0
  

       
    
#=============================================================================================================
sampleRate, aud = loadWav(r"untitledpiano.wav")

stream = Stream(audioData=aud, sampleRate=sampleRate, bufferSize=512, streamTime=3)

c = Items()
c.delayLine1 = DelayLine(0.05,stream.sampleRate, stream.nChannels)
c.delayLine2 = DelayLine(0.06,stream.sampleRate, stream.nChannels)
c.delayLine3 = DelayLine(0.07,stream.sampleRate, stream.nChannels)
c.delayLine4 = DelayLine(0.08,stream.sampleRate, stream.nChannels)
c.delayLine5 = DelayLine(0.10,stream.sampleRate, stream.nChannels)
c.delayLine6 = DelayLine(0.12,stream.sampleRate, stream.nChannels)
c.delayLine7 = DelayLine(0.14,stream.sampleRate, stream.nChannels)
c.delayLine8 = DelayLine(0.16,stream.sampleRate, stream.nChannels)
c.diffuser = Diffuse8(stream.bufferSize)

flag = True

while(flag):    
    flag = stream.startStream()
    print("Time: " + str(round(stream.time,4)) + "s")
       
    processBuffer(stream.buffer, c)
    
    stream.output()   


writeWav(r"output.wav", stream.out, sampleRate)

plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
##plt.plot(range(0,len(stream.data[0,:])),stream.data[0,:],'-',label='Stream')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
##plt.plot(range(0,len(c.delayLine1.delayBuffer[0,:])),c.delayLine1.delayBuffer[0,:],'--',label='DelayLine1')
##plt.plot(range(0,len(c.delayLine2.delayBuffer[0,:])),c.delayLine2.delayBuffer[0,:],'--',label='DelayLine2')
plt.legend(loc='upper right')
#plt.plot(range(0,len(d.channel1[0,:])),d.channel1[0,:],'-',label='Stream')
plt.show()
