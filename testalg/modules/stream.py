import numpy as np
import scipy

#=============================================================================================================
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
        
        
if __name__ == "__main__":
    main()