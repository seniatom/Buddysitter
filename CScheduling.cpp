#include "CScheduling.h"

CScheduling::~CScheduling()
{

}

void CScheduling::GetCurrentTime(int *year, int *month, int *day, int *hour, int *minute)
{
    time_t rawtime;
    struct tm * timeinfo;
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);

    *year = timeinfo->tm_year + 1900;
    *month = timeinfo->tm_mon + 1;
    *day = timeinfo->tm_mday;
    *hour = timeinfo->tm_hour;
    *minute = timeinfo->tm_min;

    return;
}

void CScheduling::MsgQueueSend(int weight)
{
    unsigned int msgprio = 1;
    my_pid = getpid();
    /* forcing specification of "-i" argument */
    if (msgprio == 0) {
        //printf("Usage: %s [-q] -p msg_prio\n", argv[0]);
        exit(1);
    }
    /* opening the queue using default attributes  --  mq_open()X */
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }
    /* producing the message */
    snprintf(msgcontent, MAX_MSG_LEN, "M %d", weight);
    /* sending the message      --  mq_send() */
    mq_send(msgq_id, msgcontent, strlen(msgcontent)+1, msgprio);
    /* closing the queue        -- mq_close() */
    mq_close(msgq_id);     
    return;
}

bool CScheduling::MsgQueueRecieve()
{
    int msgsz;
    unsigned int sender;
    struct mq_attr msgq_attr;
    
    /* opening an existing queue  --  mq_open() */
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
    if (msgq_id == (mqd_t)-1) {
        //perror("In mq_open()");
        return false;
    }
    /* getting the attributes from the queue   --  mq_getattr() */
    mq_getattr(msgq_id, &msgq_attr);

    /* getting a message */
    msgsz = mq_receive(msgq_id, msgcontent, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
        perror("In mq_receive()");
        return false;
    }
    /* closing the queue    --  mq_close() */
    mq_close(msgq_id);
    
    if (mq_unlink(MSGQOBJ_NAME) == -1)
    {
        perror("Removing queue error");
        return false;
    }
    return true;
}

void CScheduling::PrintMessage()
{
    printf("%s", msgcontent);
}

int CScheduling::GetWeight()
{
    char *token;
    token = strtok(msgcontent, " ");
    token = strtok(NULL, " ");
    return atoi(token);
}

bool CScheduling::compareTimes(int year, int month, int day, int hour, int minute, int *weight)
{

    *weight = 10;
    return true;
}