#include "CSpeaker.h"

CSpeaker::~CSpeaker()
{
    
}

void CSpeaker::InitSpeaker()
{

}

void CSpeaker::RemSpeaker()
{
    
}

void CSpeaker::StartSpeaker(int index)
{
    sleep(1);
    ConvertAudioFormat(index);
    sleep(1);
    RemoveOriginalFile(index);
    //start Speaker
}

void CSpeaker::StopSpeaker()
{
    //stop motor dd
}

void CSpeaker::ConvertAudioFormat(int index)
{
    char number[2];
    char cmd[32] = "ffmpeg -y -i audio";
    sprintf(number, "%d", index);

    strcat(cmd, number);
    char m4a[] = ".m4a";
    strcat(cmd, m4a);

    char cmd2[12] = " audio";
    
    strcat(cmd2, number); 
    char wav[] = ".wav";
    strcat(cmd2, wav);

    strcat(cmd, cmd2);

    printf("command: %s\n", cmd);

    system(cmd);

    return;
}

void CSpeaker::RemoveOriginalFile(int index)
{
    char number[2];
    char cmd[13] = "rm audio";

    sprintf(number, "%d", index);

    strcat(cmd, number);
    char m4a[] = ".m4a";
    strcat(cmd, m4a);
    
    printf("command: %s\n", cmd);
    system(cmd);

    return;
}
