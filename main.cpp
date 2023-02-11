#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <syslog.h>
#include <fstream>

using namespace std;

#include "CScheduling.h"
#include "CDatabaseHandler.h"
#define tStartupSystem_prio 1
#define tChangeWifi_prio 2
#define tStartStopSpeaker_prio 2
#define tStartStopStreaming_prio 2
#define tStartStopMotor_prio 2
#define tFlagCheck_prio 3
#define tFeedingStatus_prio 3
#define tCheckWeight_prio 4

pthread_cond_t  weight_cond         = PTHREAD_COND_INITIALIZER;
pthread_mutex_t motor_flag_m        = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t weight_m            = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t speaker_flag_m      = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t change_wifi_flag_m  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t streaming_flag_m    = PTHREAD_MUTEX_INITIALIZER;

int current_weight = 0;
int target_weight = 0;

bool motor_flag = false;
bool streaming_flag = false;
bool speaker_flag = false;
bool change_wifi_flag = false;

CScheduling scheduling;
CDatabaseHandler dbhandler;
CStreaming stream;
CMotor motor;
CSpeaker speaker;
CWeightSensor scale;

char userId[30]; //= "YW7b3hkFsaTo7qfU5gZ5tyHJ85v2";
char customId[] = "mainId";

void SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param);
void CheckFail(int status);
string ReadFile();
void WriteFile(string id);


void signal_handler(int sig) {
switch(sig) {
    case SIGTERM:
        syslog(LOG_INFO,"terminate signal catched");
        motor.StopMotor();
        stream.StopStreaming();
        exit(0);
    }
}

void *tStartupSystem (void *)
{
    while(1)
    {
        //maybe do scale calibratioN ?
    }

	return NULL;
}

void *tChangeWifi (void *arg)
{
    while(1)
    {
        if(change_wifi_flag == true)
        {
            WriteFile("0");
            exit(1);
        }
    }
	return NULL;
}

void *tStartStopSpeaker (void *arg)
{
	while(1)
    {
        if(speaker_flag == true && speaker.SpeakerStatus() == false)
        {
            speaker.StartSpeaker("BEASTARS.wav");
        }
        else if(speaker_flag == false && speaker.SpeakerStatus() == true)
        {
           // speaker.StopSpeaker();
        }
    }
	return NULL;
}

void *tStartStopStreaming (void *arg)
{
    while(1)
    {
        if(streaming_flag == true && stream.StreamStatus() == false)
        {
            stream.StartStreaming();
            printf("start streaming\n");
        }
        else if(streaming_flag == false && stream.StreamStatus() == true)
        {
            stream.StopStreaming();
            printf("stop streaming\n");
        }
    }
	return NULL;
}

void *tStartStopMotor (void *arg)
{
	while(1)
    {
        if(motor_flag == true && motor.MotorStatus() == false)
        {
            printf("starting motor");
            motor.StartMotor();
            printf("start motor\n");
        }
        else if(motor_flag == false && motor.MotorStatus() == true)
        {
            motor.StopMotor();
            printf("stop motor\n");
        }
    }
	return NULL;
}

void *tFlagCheck (void *arg) //Updates flags from database
{
	while(1)
    {
        pthread_mutex_lock(&streaming_flag_m);
        streaming_flag = dbhandler.GetStreamingFlag();
        pthread_mutex_unlock(&streaming_flag_m);

        pthread_mutex_lock(&speaker_flag_m);
        speaker_flag = dbhandler.GetStreamingFlag();
        pthread_mutex_unlock(&speaker_flag_m);

        pthread_mutex_lock(&change_wifi_flag_m);
        change_wifi_flag = dbhandler.GetStreamingFlag();
        pthread_mutex_unlock(&change_wifi_flag_m);

        sleep(1); 
    }
	return NULL;
}


void *tFeedingStatus (void *arg) //Checks for messages in msqueue
{
    while(1)
    {
        if(scheduling.MsgQueueRecieve() == true) //Checks for messages in mqueue
        {
            target_weight = scheduling.GetWeight(); //loads weight_aux with desired weight
            printf("weight: %d\n",target_weight);
        
            pthread_mutex_lock(&motor_flag_m); 
            motor_flag = true;
            printf("motor_flag = true\n"); 
            pthread_cond_wait(&weight_cond, &motor_flag_m); //waits for the desired weight
            motor_flag = false;
            printf("motor_flag = false\n");
            pthread_mutex_unlock(&motor_flag_m);
        }
    }
	return NULL;
}

void *tCheckWeight (void *arg)
{
	while(1)
    {  
        pthread_mutex_lock(&weight_m);
        //read from sensor like I did on example of hx711
        current_weight = scale.readWeightSensor();
        if(current_weight > target_weight && motor_flag == true)
        {
            pthread_cond_signal(&weight_cond); //unBlocks FeedingStatus if weight achieved
        }
        pthread_mutex_unlock(&weight_m);
        sleep(1);
    }
	return NULL;
}

int main (int argc, char *argv[])
{
	int thread_policy;
    int status;
	pthread_t StartupSystemID, ChangeWifiID, StartStopSpeakerID, StartStopStreamingID, StartStopMotorID, FlagCheckID, FeedingStatusID, CheckWeightID;

	pthread_attr_t thread_attr;
	struct sched_param thread_param;
	
    //setenv("PYTHONPATH","/home/hugo/Visual_Studio_Code/python", 1);
    setenv("PYTHONPATH","/etc/python_code", 1);

    if(ReadFile() == "0")
    {
        char id[30];
        //bluetooth implementation
        WriteFile(id);
        strcpy(userId,id);
    }else{
        ReadFile().copy(userId,29,0);
        printf("User Id: %s\n", userId);
    }

    if(!dbhandler.FirebaseInit())
    {
        printf("init error");
        return 0;
    }
    if(!dbhandler.LoginUser(userId,customId))
    {
        printf("login error");
        return 0;
    }

    system("./dScheduledTimes");

    signal(SIGTERM,signal_handler);

	pthread_attr_init (&thread_attr);
	pthread_attr_getschedpolicy (&thread_attr, &thread_policy);
	pthread_attr_getschedparam (&thread_attr, &thread_param);
	
	SetupThread(tCheckWeight_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&CheckWeightID, &thread_attr, tCheckWeight, NULL);
    CheckFail(status);

    SetupThread(tFeedingStatus_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&FeedingStatusID, &thread_attr, tFeedingStatus, NULL);
    CheckFail(status);

    SetupThread(tFlagCheck_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&FlagCheckID, &thread_attr, tFlagCheck, NULL);
    CheckFail(status);

    SetupThread(tStartStopMotor_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&StartStopMotorID, &thread_attr, tStartStopMotor, NULL);
    CheckFail(status);

    SetupThread(tStartStopStreaming_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&StartStopStreamingID, &thread_attr, tStartStopStreaming, NULL);
    CheckFail(status);

    SetupThread(tStartStopSpeaker_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&StartStopSpeakerID, &thread_attr, tStartStopSpeaker, NULL);
    CheckFail(status);

    SetupThread(tChangeWifi_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&ChangeWifiID, &thread_attr, tChangeWifi, NULL);
    CheckFail(status);

    SetupThread(tStartupSystem_prio,&thread_attr,&thread_param);
	pthread_attr_setinheritsched (&thread_attr, PTHREAD_EXPLICIT_SCHED);
	status = pthread_create (&StartupSystemID, &thread_attr, tStartupSystem, NULL);
    CheckFail(status);

	while(1)
    {}
}

void SetupThread(int prio,pthread_attr_t *pthread_attr,struct sched_param *pthread_param)
{
	pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);
	pthread_param->sched_priority = prio;
	pthread_attr_setschedparam (pthread_attr, pthread_param);
}

void CheckFail(int status)
{
	if(status)
	{
		if(status==EPERM)
			fprintf(stderr,"pthread_create() got EPERM\n");
		else if(status==EINVAL)
      			fprintf(stderr,"pthread_create() got EINVAL\n");
    		else
      			fprintf(stderr,"pthread_create() got neither EPERM nor EINVAL\n");

    		fprintf(stderr,"pthread_create() got error %d\n",status);
    		errno=status;
    		perror("pthread_create()");
    		exit(1);
  	}
}

string ReadFile()
{
    string text;
    char* string;
    fstream MyReadFile("Paired_Status");
    getline(MyReadFile,text);
    MyReadFile.close();

    return text;
}

void WriteFile(string id)
{
    string text;
    char* string;
    fstream MyReadFile("Paired_Status");
    MyReadFile << id;
    MyReadFile.close();
    
    return;
}