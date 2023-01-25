#include "CScheduling.h"

void CScheduling::GetCurrentTime(int *year, int *month, int *day, int *hour, int *minute)
{
    time_t rawtime;
    struct tm * timeinfo;
    
    time (&rawtime);
    timeinfo = localtime(&rawtime); //current system time

    *year = timeinfo->tm_year + 1900; //returns year after 1900
    *month = timeinfo->tm_mon + 1; //returns month 0-11
    *day = timeinfo->tm_mday;
    *hour = timeinfo->tm_hour;
    *minute = timeinfo->tm_min;

}

void CScheduling::MsgQueueSend(int weight)
{
    unsigned int msgprio = 1;
    my_pid = getpid();

    if (msgprio == 0) {
        exit(1);
    }

    /* opening the queue using default attributes */
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }
    
    snprintf(msgcontent, MAX_MSG_LEN, "M %d", weight);
    /* sending the message */
    mq_send(msgq_id, msgcontent, strlen(msgcontent)+1, msgprio);
    /* closing the queue */
    mq_close(msgq_id);     
}

bool CScheduling::MsgQueueRecieve()
{
    int msgsz;
    unsigned int sender;
    struct mq_attr msgq_attr;
    
    /* opening an existing queue */
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
    if (msgq_id == (mqd_t)-1) {
        //perror("In mq_open()");
        return false; //no msg in queue
    }
    /* getting the attributes from the queue */
    mq_getattr(msgq_id, &msgq_attr);

    /* getting a message */
    msgsz = mq_receive(msgq_id, msgcontent, MAX_MSG_LEN, &sender);
    if (msgsz == -1) {
        perror("In mq_receive()");
        return false;
    }
    /* closing the queue */
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

int CScheduling::GetWeight() //parse msgcontent
{
    char *token;
    token = strtok(msgcontent, " ");
    token = strtok(NULL, " ");
    return atoi(token);
}
