#include "CStreaming.h"

CStreaming::~CStreaming()
{

}

void CStreaming::StartStreaming()
{
    if(Streaming_Status == false) //RTMP stream to nginx
    {   
        /* /dev/video -> camera address  */
        system("ffmpeg -re \
        -f v4l2 -video_size 320x240 -thread_queue_size 16384 -i /dev/video0 \
        -c:a copy -c:v h264 -b:v 2048k -preset ultrafast -filter:v fps=fps=30 -tune zerolatency \
        -f flv rtmp:localhost/live \
        -nostdin -nostats > /var/log/ffmpeg_output.log 2>&1 < /dev/null &"); //streams to rtmp:localhost/live

        Streaming_Status = true;
    }
}

void CStreaming::StopStreaming()
{
    system("pidof ffmpeg | xargs kill -9"); //kills ffmpeg command
    Streaming_Status = false;
}

bool CStreaming::StreamStatus()
{
    return Streaming_Status;
}
