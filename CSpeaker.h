#ifndef SPEAKER_H
#define SPEAKER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/wait.h>
#include <bits/stdc++.h> 

class CSpeaker
{
public:
    CSpeaker();
    ~CSpeaker();
    void StartSpeaker(int index);
    bool SpeakerStatus();

private:
    void ConvertAudioFormat(int index);
    void RemoveOriginalFile(int index);
    void PlayAudio(char wavfile[]);
};

#endif
