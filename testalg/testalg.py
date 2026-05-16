import numpy as np
import matplotlib.pyplot as plt
import scipy
import time
import yaml

#=============================================================================================================   
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


class WaveFile:        
    @staticmethod    
    def load(file: str):
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
        
    @staticmethod   
    def write(filename: str, data: np.ndarray, sampleRate: int, bitDepth: str):
        if (bitDepth == 'int16'):
            dtype = np.int16
        elif(bitDepth == 'int32'):
            dtype = np.int32
        else:
            raise ValueError("Bit-depth not supported.")
            
        amplitude = np.iinfo(dtype).max
        writeData = amplitude * data
        print("Peaks: " + str(np.min(writeData)) + "," + str(np.max(writeData)))
        writeData = np.clip(writeData, -amplitude, amplitude).T   
        scipy.io.wavfile.write(filename, sampleRate, writeData.astype(dtype))   


class Items:
        pass

        
class DelayLine:
    def __init__(self, delayTime: float, sampleRate: int, nChannels: int):
        self.delaySamples = int(delayTime*sampleRate)
        self.delayBufferSize = 5 * sampleRate
        self.delayBuffer = np.ndarray((nChannels, self.delayBufferSize), np.float32)
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
                    
    def addToBuffer(self, buffer: np.ndarray, gain: float):
        bufferSize = len(buffer[0,:])
        readPosition = self.writePosition - self.delaySamples
        
        if (readPosition < 0):
            readPosition = readPosition + self.delayBufferSize
            
        if (readPosition + bufferSize < self.delayBufferSize):
            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize):
                    buffer[channel,i] = buffer[channel,i] + self.delayBuffer[channel,readPosition+i]*gain
        else:
            end = self.delayBufferSize - readPosition 
            for channel in range(0, self.nChannels):
                for i in range(0,end):
                    buffer[channel,i] = buffer[channel,i] + self.delayBuffer[channel,readPosition+i]*gain

            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize-end):
                    buffer[channel,end+i] = buffer[channel,end+i] + self.delayBuffer[channel,i]*gain
                    
                    
    def copyToBuffer(self, buffer: np.ndarray, gain: float):
        bufferSize = len(buffer[0,:])
        readPosition = self.writePosition - self.delaySamples
        
        if (readPosition < 0):
            readPosition = readPosition + self.delayBufferSize
            
        if (readPosition + bufferSize < self.delayBufferSize):
            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize):
                    buffer[channel,i] = self.delayBuffer[channel,readPosition+i]*gain
        else:
            end = self.delayBufferSize - readPosition 
            for channel in range(0, self.nChannels):
                for i in range(0,end):
                    buffer[channel,i] = self.delayBuffer[channel,readPosition+i]*gain

            for channel in range(0, self.nChannels):
                for i in range(0,bufferSize-end):
                    buffer[channel,end+i] = self.delayBuffer[channel,i]*gain
                            
        
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
                exec(f"self.bufferMatrix[self.nChannels*{i}+{channel},:] = d.buffer{i}[{channel},:]",locals())
                    
        for i in range(0,self.diffuserChannels):
            exec(f"d.buffer{str(i)}.fill(0)",locals())        
        
        result = np.dot(self.mixMatrix, self.bufferMatrix)
                        
        for i in range(0,self.diffuserChannels):
            for channel in range(0,self.nChannels):
                exec(f"d.buffer{str(i)}[{channel},:] = result[{self.nChannels*i+channel},:]",locals()) 
                    

class ReverbStage():
    def __init__(self, bufferSize: int, sampleRate: int, nChannels: int, params, name: str):
        self.bufferSize = bufferSize
        self.sampleRate = sampleRate
        self.nChannels = nChannels
        self.name = name
        self.params = params
        
        self.bufferForDiffusion = Items()
        for i in range(0,8):
            exec(f"self.bufferForDiffusion.buffer{str(i)} = np.zeros((self.nChannels, self.bufferSize))")
            
        self.bufferForFdBkDelay = Items()
        for i in range(0,8):
            exec(f"self.bufferForFdBkDelay.buffer{str(i)} = np.zeros((self.nChannels, self.bufferSize))")
            
        self.delayLine0 = DelayLine(self.params['delayLine0']['time'],self.sampleRate, self.nChannels)
        self.delayLine1 = DelayLine(self.params['delayLine1']['time'],self.sampleRate, self.nChannels)
        self.delayLine2 = DelayLine(self.params['delayLine2']['time'],self.sampleRate, self.nChannels)
        self.delayLine3 = DelayLine(self.params['delayLine3']['time'],self.sampleRate, self.nChannels)
        self.delayLine4 = DelayLine(self.params['delayLine4']['time'],self.sampleRate, self.nChannels)
        self.delayLine5 = DelayLine(self.params['delayLine5']['time'],self.sampleRate, self.nChannels)
        self.delayLine6 = DelayLine(self.params['delayLine6']['time'],self.sampleRate, self.nChannels)
        self.delayLine7 = DelayLine(self.params['delayLine7']['time'],self.sampleRate, self.nChannels)
        
        
        self.diffuser = Diffuse8(self.bufferSize)
        
        self.delayLineFdBk0 = DelayLine(self.params['delayLineFdBk0']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk1 = DelayLine(self.params['delayLineFdBk1']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk2 = DelayLine(self.params['delayLineFdBk2']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk3 = DelayLine(self.params['delayLineFdBk3']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk4 = DelayLine(self.params['delayLineFdBk4']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk5 = DelayLine(self.params['delayLineFdBk5']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk6 = DelayLine(self.params['delayLineFdBk6']['time'],self.sampleRate, self.nChannels)
        self.delayLineFdBk7 = DelayLine(self.params['delayLineFdBk7']['time'],self.sampleRate, self.nChannels)
        
            
    def process(self, buffer: np.ndarray):
        for i in range(0,8):
            exec(f"self.bufferForDiffusion.buffer{str(i)}.fill(0)",locals())
        
        for i in range(0,8):
            exec(f"self.bufferForFdBkDelay.buffer{str(i)}.fill(0)",locals())
    
        print("INFO:",self.name,"Populating delay buffers")   
        
        splitGain = self.params['splitGain']
        
        self.delayLine0.delay(buffer)
        self.delayLine0.copyToBuffer(self.bufferForDiffusion.buffer0,splitGain)
        self.delayLine1.delay(buffer)
        self.delayLine1.copyToBuffer(self.bufferForDiffusion.buffer1,splitGain)
        self.delayLine2.delay(buffer)
        self.delayLine2.copyToBuffer(self.bufferForDiffusion.buffer2,splitGain)
        self.delayLine3.delay(buffer)
        self.delayLine3.copyToBuffer(self.bufferForDiffusion.buffer3,splitGain)
        self.delayLine4.delay(buffer)
        self.delayLine4.copyToBuffer(self.bufferForDiffusion.buffer4,splitGain)
        self.delayLine5.delay(buffer)
        self.delayLine5.copyToBuffer(self.bufferForDiffusion.buffer5,splitGain)
        self.delayLine6.delay(buffer)
        self.delayLine6.copyToBuffer(self.bufferForDiffusion.buffer6,splitGain)
        self.delayLine7.delay(buffer)
        self.delayLine7.copyToBuffer(self.bufferForDiffusion.buffer7,splitGain)
    
        print("INFO:",self.name,"Mixing buffers")   
        self.diffuser.diffuse(self.bufferForDiffusion)
        
        print("INFO:",self.name,"Feedback Delay")
        np.add(self.bufferForDiffusion.buffer0,self.bufferForFdBkDelay.buffer0,out=self.bufferForFdBkDelay.buffer0)
        np.add(self.bufferForDiffusion.buffer1,self.bufferForFdBkDelay.buffer1,out=self.bufferForFdBkDelay.buffer1)
        np.add(self.bufferForDiffusion.buffer2,self.bufferForFdBkDelay.buffer2,out=self.bufferForFdBkDelay.buffer2)
        np.add(self.bufferForDiffusion.buffer3,self.bufferForFdBkDelay.buffer3,out=self.bufferForFdBkDelay.buffer3)
        np.add(self.bufferForDiffusion.buffer4,self.bufferForFdBkDelay.buffer4,out=self.bufferForFdBkDelay.buffer4)
        np.add(self.bufferForDiffusion.buffer5,self.bufferForFdBkDelay.buffer5,out=self.bufferForFdBkDelay.buffer5)
        np.add(self.bufferForDiffusion.buffer6,self.bufferForFdBkDelay.buffer6,out=self.bufferForFdBkDelay.buffer6)
        np.add(self.bufferForDiffusion.buffer7,self.bufferForFdBkDelay.buffer7,out=self.bufferForFdBkDelay.buffer7)
        
        gain = self.params['feedbackGain']
        
        self.delayLineFdBk0.addToBuffer(self.bufferForFdBkDelay.buffer0,gain)
        self.delayLineFdBk0.delay(self.bufferForFdBkDelay.buffer0)
        
        self.delayLineFdBk1.addToBuffer(self.bufferForFdBkDelay.buffer1,gain)
        self.delayLineFdBk1.delay(self.bufferForFdBkDelay.buffer1)
        
        self.delayLineFdBk2.addToBuffer(self.bufferForFdBkDelay.buffer2,gain)
        self.delayLineFdBk2.delay(self.bufferForFdBkDelay.buffer2)    
        
        self.delayLineFdBk3.addToBuffer(self.bufferForFdBkDelay.buffer3,gain)
        self.delayLineFdBk3.delay(self.bufferForFdBkDelay.buffer3)    
        
        self.delayLineFdBk4.addToBuffer(self.bufferForFdBkDelay.buffer4,gain)
        self.delayLineFdBk4.delay(self.bufferForFdBkDelay.buffer4)
        
        self.delayLineFdBk5.addToBuffer(self.bufferForFdBkDelay.buffer5,gain)
        self.delayLineFdBk5.delay(self.bufferForFdBkDelay.buffer5)
        
        self.delayLineFdBk6.addToBuffer(self.bufferForFdBkDelay.buffer6,gain)
        self.delayLineFdBk6.delay(self.bufferForFdBkDelay.buffer6)
        
        self.delayLineFdBk7.addToBuffer(self.bufferForFdBkDelay.buffer7,gain)
        self.delayLineFdBk7.delay(self.bufferForFdBkDelay.buffer7)
            
            
        for k in range(0,8):
            exec(f"np.add(buffer,self.params['mixGain']*self.bufferForFdBkDelay.buffer{k},out=buffer)",locals(),globals())
        return 0


class Reverb:
    def __init__(self, configFile: str, bufferSize: int, sampleRate: int, nChannels: int):
        self.bufferSize = bufferSize
        self.sampleRate = sampleRate
        self.nChannels = nChannels
        
        with open(configFile, 'r') as file:
            self.data = dict(yaml.safe_load(file))
        
        for i in range(0, self.data['Reverb']['stages']):
            exec(f"self.params = self.data['Reverb']['Stage{i+1}']", locals())
            rvb = ReverbStage(self.bufferSize, self.sampleRate, self.nChannels, self.params, name='Stage'+str(i+1))
            exec(f"self.reverb{i+1} = rvb", locals())
            
    def process(self, buffer: np.ndarray):
        for i in range(0, self.data['Reverb']['stages']):
            exec(f"self.reverb{i+1}.process(buffer)", locals())
            
        
#=============================================================================================================
sampleRate, aud = WaveFile.load(r"untitledpiano.wav")

stream = Stream(audioData=aud, sampleRate=sampleRate, bufferSize=512, streamTime=5)

reverb = Reverb('settings.yaml', stream.bufferSize, stream.sampleRate, stream.nChannels)

flag = True

while(flag):    
    flag = stream.startStream()
    print("Time:",str(round(stream.time,4)),"s")
    starttime = time.process_time()
    reverb.process(stream.buffer)
    endtime = time.process_time()
    stream.output()   
    print("ExecTime:",str(endtime-starttime))

WaveFile.write(r"output.wav", stream.out, sampleRate, 'int32')

plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
plt.legend(loc='upper right')
plt.show()
