#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <unistd.h> 

#define MSGQOBJ_NAME    "/ScheduledTime"
#define MAX_MSG_LEN     10000

class CScheduling {
    public:
        void GetCurrentTime(int *year, int *month, int *day, int *hour, int *minute);
        void MsgQueueSend(int weight);
        bool MsgQueueRecieve();
        int GetWeight();
        void PrintMessage();
    
    private:
        mqd_t msgq_id;
        pid_t my_pid;
        char msgcontent[MAX_MSG_LEN];
};

#endif
