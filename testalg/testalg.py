import numpy as np
import random
import math
import matplotlib.pyplot as plt

gSampleRate = 48000
gBuffLen = 512
gBuff = np.zeros(gBuffLen)
gAllpassBuff = np.zeros(3*gSampleRate)
gSecsOfAudio = 10
PI = 3.1416

def sineArray(freq):
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
    
    
def stageBuffer(aud,start):
    global gBuff
    for i in range(0,gBuffLen):
        gBuff[i] = aud[start+1]
        assert(len(gBuff) == gBuffLen)
    
 
    
#def yourAlg(buffer):
    #do Something

    
#=============================================================================================================   
aud = sineArray(10)
samples = len(aud)
start = 0

while(samples > 0):
    
    stageBuffer(aud,start)
    
    #allPass(delaySamples)
    
    start = start + gBuffLen
    samples = samples - gBuffLen
    


    

    
    