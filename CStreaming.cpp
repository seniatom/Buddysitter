#include "CStreaming.h"

CStreaming::~CStreaming()
{

}

void CStreaming::StartStreaming()
{
    if(Streaming_Status == false)
    {
        system("ffmpeg -re         -f v4l2 -video_size 320x240 -thread_queue_size 16384 -i /dev/video0         -c:a copy -c:v h264 -b:v 2048k -preset ultrafast -filter:v fps=fps=30 -tune zerolatency         -f flv rtmp:localhost/live");
    }
    Streaming_Status = true;
}

void CStreaming::StopStreaming()
{
    system("pidof ffmpeg | xargs kill -9");
    Streaming_Status = false;
}

bool CStreaming::StreamStatus()
{
    return Streaming_Status;
}