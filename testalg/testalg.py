import numpy as np
import random
import math
import matplotlib.pyplot as plt

gSampleRate = 48000
gBuffLen = 512
gChannels = 2
gBuff = np.zeros((gChannels, gBuffLen), np.float32)
gAllpassBuff = np.zeros(3*gSampleRate)
gSecsOfAudio = 10
PI = 3.1416


#=============================================================================================================        
def sineArray(freq: float):
    arr = []
    k = 0
    time = []
    for i in range (0, gSecsOfAudio * gSampleRate):
        arr.append(math.sin(2 * PI * freq * k))
        k = k + 1/gSampleRate
        time.append(k)
    #plt.plot(time, arr)
    #plt.show()
    return np.array(arr)
    
    
def stageBuffer(aud: np.ndarray, readPos: int, channel: int):
    global gBuff   
    for i in range(0,gBuffLen):
        gBuff[channel,i] = aud[readPos+i]
        assert(len(gBuff[channel,:]) == gBuffLen)
    

def writeOutput(buff: np.ndarray, writePos: int, channel: int):
    global output
    for i in range(0,len(buff[channel,:])):
        output[channel, writePos+i] = buff[channel, i]
        
    
def processBuffer(buff: np.ndarray):
    # youtAlg
    return 0

    
#=============================================================================================================   
aud = sineArray(10)
samples = len(aud)
start = 0
output = np.zeros((gChannels, len(aud)), np.float32)


while(samples > 0):
    if (start > len(aud)-gSampleRate):
        print("Finished")
        break
    
    for channel in range(0, gChannels):
        stageBuffer(aud, start, channel)
    
    processBuffer(gBuff)
    
    for channel in range(0, gChannels):
        writeOutput(gBuff, start, channel)
    
    start = start + gBuffLen
    samples = samples - gBuffLen

    
plt.plot(range(0,len(aud)),aud[:],'-',label='Input')
plt.plot(range(0,len(aud)),output[0,:],'--',label='Output')
plt.legend(loc='upper right')
plt.show()
    

    
    