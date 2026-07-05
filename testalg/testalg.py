from modules import stream as st
from modules import timefx as tf
import matplotlib.pyplot as plt
import time
        
#=============================================================================================================

sampleRate, aud = st.WaveFile.load(r"untitledpiano.wav")

stream = st.Stream(audioData=aud, sampleRate=sampleRate, bufferSize=512, streamTime=5)

reverb = tf.ReverbChain('settings.yaml', stream.bufferSize, stream.sampleRate, stream.nChannels)

flag = True

while(flag):    
    flag = stream.startStream()
    print("Time:",str(round(stream.time,4)),"s")
    starttime = time.process_time()
    reverb.process(stream.buffer)
    endtime = time.process_time()
    stream.output()   
    print("ExecTime:",str(endtime-starttime))

st.WaveFile.write(r"output.wav", stream.out, sampleRate, 'int32')

plt.plot(range(0,len(aud[0,:])),aud[0,:],'-',label='WaveFile')
plt.plot(range(0,len(stream.out[0,:])),stream.out[0,:],'--',label='Output',linewidth=0.5)
plt.legend(loc='upper right')
plt.show()
