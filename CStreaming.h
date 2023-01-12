#ifndef STREAMING_H
#define STREAMING_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/wait.h>

class CStreaming
{
public:
    ~CStreaming();
    void StartStreaming();
    void StopStreaming();
    bool StreamStatus();

private:
    bool Streaming_Status = false;
};

#endif