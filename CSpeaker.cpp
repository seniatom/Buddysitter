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

void CSpeaker::StartSpeaker()
{
    if(speaker_status == false)
    {
        //start motor dd
    }
    speaker_status = true;
}

void CSpeaker::StopSpeaker()
{
    //stop motor dd
    speaker_status = false;
}

bool CSpeaker::SpeakerStatus()
{
    return speaker_status;
}
