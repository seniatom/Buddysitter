#include "CSpeaker.h"

CSpeaker::~CSpeaker()
{
    printf("\n\nRemoving Device Driver CSpeaker\n");
    system("modprobe -r snd_bcm2835");
}

CSpeaker::CSpeaker()
{
    printf("\n\nInserting Device Driver CSpeaker\n");
    system("modprobe snd_bcm2835");
}

void CSpeaker::StartSpeaker(char wavfile[])
{
    int command_size = 20;
    char command[command_size] = "play ";
    
    for(int i = 0; i<command_size; i++)
    {
        command[5+i] = wavfile[i];
    }

    speaker_status = true;
    system(command);
    speaker_status = false;
}

bool CSpeaker::SpeakerStatus()
{
    return speaker_status;
}