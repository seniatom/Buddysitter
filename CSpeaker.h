#ifndef SPEAKER_H
#define SPEAKER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/wait.h>

class CSpeaker
{
public:
    ~CSpeaker();
    void InitSpeaker();
    void RemSpeaker();
    void StartSpeaker();
    void StopSpeaker();
    bool SpeakerStatus();

private:
    bool speaker_status = false;
};

#endif