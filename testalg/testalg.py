import numpy as np
import random
import math
import matplotlib.pyplot as plt
import scipy
import time

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
                    
                    
    def copyToBuffer(self, buffer: np.ndarray, gain: float):
        bufferSize = len(buffer[0,:])
        self.readPosition = self.writePosition - self.delaySamples
        
        if (self.readPosition < 0):
            self.readPosition = self.readPosition + self.delayBufferSize
            
        if (self.readPosition + bufferSize < self.delayBufferSize):
            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize):
                    buffer[channel,i] = self.delayBuffer[channel,self.readPosition+i]*gain
        else:
            end = self.delayBufferSize - self.readPosition 
            for channel in range(0, self.nChannels):
                for i in range(0,end):
                    buffer[channel,i] = self.delayBuffer[channel,self.readPosition+i]*gain

            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize-end):
                    buffer[channel,end+i] = self.delayBuffer[channel,0+i]*gain
                            

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
                                   [1, 1,-1,-1,-1,-1, 1, 1, 1, 1,-1,-1,-1,-1, 1, 1],
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

        for i in range(0,self.diffuserChannels):
            for channel in range(0,self.nChannels):         
                exec(f"self.bufferMatrix[2*{i}+{channel},:] = d.buffer{i}[{channel},:]",locals())
                    
        for i in range(0,self.diffuserChannels):
            exec(f"d.buffer{str(i)}.fill(0)",locals())        
        
        result = np.dot(self.mixMatrix, self.bufferMatrix)
                        
        for i in range(0,self.diffuserChannels):
            for channel in range(0,self.nChannels):
                exec(f"d.buffer{str(i)}[{channel},:] = result[{2*i+channel},:]",locals()) 
                    
                    
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
    
    
def processBuffer(buff: np.ndarray, proc: Items):
    
    for i in range(0,8):
        exec(f"proc.bufferForDiffusion.buffer{str(i)}.fill(0)",locals())
    
    for i in range(0,8):
        exec(f"proc.bufferForFdBkDelay.buffer{str(i)}.fill(0)",locals())
   
    print("INFO: Populating delay buffers")   
    
    splitGain = 0.0125
    
    proc.delayLine0.delay(buff)
    proc.delayLine0.copyToBuffer(proc.bufferForDiffusion.buffer0,splitGain)
    proc.delayLine1.delay(buff)
    proc.delayLine1.copyToBuffer(proc.bufferForDiffusion.buffer1,splitGain)
    proc.delayLine2.delay(buff)    
    proc.delayLine2.copyToBuffer(proc.bufferForDiffusion.buffer2,splitGain)
    proc.delayLine3.delay(buff)    
    proc.delayLine3.copyToBuffer(proc.bufferForDiffusion.buffer3,splitGain)
    proc.delayLine4.delay(buff)
    proc.delayLine4.copyToBuffer(proc.bufferForDiffusion.buffer4,splitGain)
    proc.delayLine5.delay(buff)
    proc.delayLine5.copyToBuffer(proc.bufferForDiffusion.buffer5,splitGain)
    proc.delayLine6.delay(buff)
    proc.delayLine6.copyToBuffer(proc.bufferForDiffusion.buffer6,splitGain)
    proc.delayLine7.delay(buff)
    proc.delayLine7.copyToBuffer(proc.bufferForDiffusion.buffer7,splitGain)

    print("INFO: Mixing buffers")   
    proc.diffuser.diffuse(proc.bufferForDiffusion)
    
    print("INFO: Feedback Delay")
    np.add(proc.bufferForDiffusion.buffer0,proc.bufferForFdBkDelay.buffer0,out=proc.bufferForFdBkDelay.buffer0)
    np.add(proc.bufferForDiffusion.buffer1,proc.bufferForFdBkDelay.buffer1,out=proc.bufferForFdBkDelay.buffer1)
    np.add(proc.bufferForDiffusion.buffer2,proc.bufferForFdBkDelay.buffer2,out=proc.bufferForFdBkDelay.buffer2)
    np.add(proc.bufferForDiffusion.buffer3,proc.bufferForFdBkDelay.buffer3,out=proc.bufferForFdBkDelay.buffer3)
    np.add(proc.bufferForDiffusion.buffer4,proc.bufferForFdBkDelay.buffer4,out=proc.bufferForFdBkDelay.buffer4)
    np.add(proc.bufferForDiffusion.buffer5,proc.bufferForFdBkDelay.buffer5,out=proc.bufferForFdBkDelay.buffer5)
    np.add(proc.bufferForDiffusion.buffer6,proc.bufferForFdBkDelay.buffer6,out=proc.bufferForFdBkDelay.buffer6)
    np.add(proc.bufferForDiffusion.buffer7,proc.bufferForFdBkDelay.buffer7,out=proc.bufferForFdBkDelay.buffer7)
    
    gain = 0.8
    
    proc.delayLineFdBk0.addToBuffer(proc.bufferForFdBkDelay.buffer0,gain)
    proc.delayLineFdBk0.delay(proc.bufferForFdBkDelay.buffer0)
    
    proc.delayLineFdBk1.addToBuffer(proc.bufferForFdBkDelay.buffer1,gain)
    proc.delayLineFdBk1.delay(proc.bufferForFdBkDelay.buffer1)
        
    proc.delayLineFdBk2.addToBuffer(proc.bufferForFdBkDelay.buffer2,gain)
    proc.delayLineFdBk2.delay(proc.bufferForFdBkDelay.buffer2)    
        
    proc.delayLineFdBk3.addToBuffer(proc.bufferForFdBkDelay.buffer3,gain)
    proc.delayLineFdBk3.delay(proc.bufferForFdBkDelay.buffer3)    
        
    proc.delayLineFdBk4.addToBuffer(proc.bufferForFdBkDelay.buffer4,gain)
    proc.delayLineFdBk4.delay(proc.bufferForFdBkDelay.buffer4)
        
    proc.delayLineFdBk5.addToBuffer(proc.bufferForFdBkDelay.buffer5,gain)
    proc.delayLineFdBk5.delay(proc.bufferForFdBkDelay.buffer5)
        
    proc.delayLineFdBk6.addToBuffer(proc.bufferForFdBkDelay.buffer6,gain)
    proc.delayLineFdBk6.delay(proc.bufferForFdBkDelay.buffer6)
    
    proc.delayLineFdBk7.addToBuffer(proc.bufferForFdBkDelay.buffer7,gain)
    proc.delayLineFdBk7.delay(proc.bufferForFdBkDelay.buffer7)
        
        
    for k in range(0,8):
        exec(f"np.add(buff,proc.bufferForFdBkDelay.buffer{k},out=buff)",locals(),globals())
    return 0

   
#=============================================================================================================
sampleRate, aud = loadWav(r"untitledpiano.wav")

stream = Stream(audioData=aud, sampleRate=sampleRate, bufferSize=512, streamTime=5)

c = Items()
c.sampleRate = stream.sampleRate
c.bufferSize = stream.bufferSize
c.nChannels = stream.nChannels

c.bufferForDiffusion = Items()

for i in range(0,8):
    exec(f"c.bufferForDiffusion.buffer{str(i)} = np.zeros((stream.nChannels, stream.bufferSize))")
    
c.bufferForFdBkDelay = Items()

for i in range(0,8):
    exec(f"c.bufferForFdBkDelay.buffer{str(i)} = np.zeros((stream.nChannels, stream.bufferSize))")
    
c.delayLine0 = DelayLine(0.06,stream.sampleRate, stream.nChannels)
c.delayLine1 = DelayLine(0.05,stream.sampleRate, stream.nChannels)
c.delayLine2 = DelayLine(0.09,stream.sampleRate, stream.nChannels)
c.delayLine3 = DelayLine(0.07,stream.sampleRate, stream.nChannels)
c.delayLine4 = DelayLine(0.08,stream.sampleRate, stream.nChannels)
c.delayLine5 = DelayLine(0.10,stream.sampleRate, stream.nChannels)
c.delayLine6 = DelayLine(0.12,stream.sampleRate, stream.nChannels)
c.delayLine7 = DelayLine(0.14,stream.sampleRate, stream.nChannels)


c.diffuser = Diffuse8(stream.bufferSize)

c.delayLineFdBk0 = DelayLine(0.02,stream.sampleRate, stream.nChannels)
c.delayLineFdBk1 = DelayLine(0.03,stream.sampleRate, stream.nChannels)
c.delayLineFdBk2 = DelayLine(0.04,stream.sampleRate, stream.nChannels)
c.delayLineFdBk3 = DelayLine(0.05,stream.sampleRate, stream.nChannels)
c.delayLineFdBk4 = DelayLine(0.06,stream.sampleRate, stream.nChannels)
c.delayLineFdBk5 = DelayLine(0.07,stream.sampleRate, stream.nChannels)
c.delayLineFdBk6 = DelayLine(0.09,stream.sampleRate, stream.nChannels)
c.delayLineFdBk7 = DelayLine(0.12,stream.sampleRate, stream.nChannels)


flag = True

while(flag):    
    flag = stream.startStream()
    print("Time: " + str(round(stream.time,4)) + "s")
    starttime = time.process_time()
    processBuffer(stream.buffer, c)
    endtime = time.process_time()
    stream.output()   
    print("ExecTime: " + str(endtime-starttime))

writeWav(r"output.wav", stream.out, sampleRate)

plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
##plt.plot(range(0,len(stream.data[0,:])),stream.data[0,:],'-',label='Stream')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
##plt.plot(range(0,len(c.delayLine1.delayBuffer[0,:])),c.delayLine1.delayBuffer[0,:],'--',label='DelayLine1')
##plt.plot(range(0,len(c.delayLine2.delayBuffer[0,:])),c.delayLine2.delayBuffer[0,:],'--',label='DelayLine2')
plt.legend(loc='upper right')
#plt.plot(range(0,len(d.channel1[0,:])),d.channel1[0,:],'-',label='Stream')
plt.show()
