#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <unistd.h> 

/* name of the POSIX object referencing the queue */
#define MSGQOBJ_NAME    "/ScheduledTime"
/* max length of a message (just for this process) */
#define MAX_MSG_LEN     10000

using namespace std;
using std::string;

class CScheduling {
    public:
        void GetCurrentTime(int *year, int *month, int *day, int *hour, int *minute);
        void MsgQueueSend(int weight);
        bool MsgQueueRecieve();
        int GetWeight();
        void PrintMessage();
        bool compareTimes(int year, int month, int day, int hour, int minute, int wait);
    
    private:
        mqd_t msgq_id;
        pid_t my_pid;
        char msgcontent[MAX_MSG_LEN];
};

#endif