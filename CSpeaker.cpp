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

void CSpeaker::StartSpeaker(int index) 
{
    sleep(1);
    /* Coverts from .m4a file to .wav */
    ConvertAudioFormat(index); 
    sleep(1);
    /* Removes .m4a file */
    RemoveOriginalFile(index);
    
    /* audio(index).wav string */
    char audiofile[10] = "audio";
    strcat(audiofile, index); 
    char wav[] = ".wav";
    strcat(audiofile, wav);
    
    PlayAudio(audiofile);
}

void CSpeaker::PlayAudio(char wavfile[])
{
    nt command_size = 20;
    char command[command_size] = "play ";
    
    for(int i = 0; i<command_size; i++)
    {
        command[5+i] = wavfile[i];
    }

    speaker_status = true;
    system(command);
    speaker_status = false;
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
