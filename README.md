Delay plugin made with JUCE Framework (v7.0.12).

Parameter controls:
1) Delay Time: 0 to 2 seconds [L and R channels].
2) BPM Sync delay
3) Feedback Gain: 0 to 1 [L and R channels].
4) Dry/Wet mix
5) Modulated Delay

![v1 3 0](https://github.com/user-attachments/assets/ab4f0723-c9cc-4003-815f-8742675d430e)


Instructions to compile the source:
- Prerequisites: Download and install VisualStudio2022
1) Clone the DelayPlugin repo.
2) Download and unpack [JUCE](https://juce.com/download/).
3) Launch Projucer.exe from the JUCE folder you just unpacked.
4) In Projucer, click on File->Open. Open Delay.jucer in DelayPlugin repo.
   
   ![Projucer](https://github.com/user-attachments/assets/4bef6907-320f-46dd-b010-a56642b04c0f)

5) Choose VisualStudio2022 from the drop down menu to open the project.
6) To build the VST3 plugin, right click on Delay_VST3 in the Solution Explorer and choose Build.
7) Copy the Delay.vst3 from the Builds folder to VST3 folder of your DAW. The location of the .vst3 file is $reporoot\Builds\VisualStudio2022\x64\Debug\VST3\Delay.vst3\Contents\x86_64-win.
8) To build the Standalone plugin, right click on Delay_StandalonePlugin in the Solution Explorer and choose Build. The standalone plugin Delay.exe can be found in the directory $reporoot\Builds\VisualStudio2022\x64\Debug\Standalone Plugin
   
![Solution](https://github.com/user-attachments/assets/4e966c11-2093-45e7-8066-1656e79bbab1)


Credits:
The Audio Programmer: https://www.youtube.com/@TheAudioProgrammer


