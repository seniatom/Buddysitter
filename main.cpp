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

#include "CScheduling.cpp"
#include "CDatabaseHandler.cpp"
#include "CStreaming.cpp"
#include "CMotor.cpp"
#include "CSpeaker.cpp"
#include "CHX711.cpp"

#define tChangeWifi_prio 2
#define tStartStopSpeaker_prio 2
#define tStartStopStreaming_prio 2
#define tStartStopMotor_prio 2
#define tFlagCheck_prio 4//2
#define tFeedingStatus_prio 3
#define tCheckWeight_prio 4

pthread_cond_t  weight_cond         = PTHREAD_COND_INITIALIZER;
pthread_mutex_t motor_flag_m        = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t weight_m            = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t speaker_flag_m      = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t change_wifi_flag_m  = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t streaming_flag_m    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t firebase_m          = PTHREAD_MUTEX_INITIALIZER;

int weight_var = 100;
int weight_aux = 0;

int motor_flag = 0;
int streaming_flag = 0;
int speaker_flag = 0;
int change_wifi_flag = 0;

CScheduling scheduling;
CDatabaseHandler dbhandler;
CStreaming stream;
CMotor motor;
CSpeaker speaker;
CHX711 sensor;

char userId[30]; //= "YW7b3hkFsaTo7qfU5gZ5tyHJ85v2";
char main_email[] = "main.admin@buddysitter.com";
char main_password[] = "123456";

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
        scheduling.~CScheduling();
        stream.~CStreaming();
        motor.~CMotor();
        sensor.~CHX711();
        Py_FinalizeEx();
        exit(0);
    }
}

void *tChangeWifi (void *arg)
{
    while(1)
    {
        if(change_wifi_flag == 1)
        {
            WriteFile("0");
            exit(1);
        }
        sleep(1);
    }
	return NULL;
}

void *tStartStopSpeaker (void *arg)
{
	while(1)
    {
        if(speaker_flag != 0)
        {
            pthread_mutex_lock(&firebase_m);
            printf("start reset\n");
            dbhandler.ResetAudioValue();
            printf("start downdload\n");
            dbhandler.DownloadAudio(speaker_flag);
            pthread_mutex_unlock(&firebase_m);

            pthread_mutex_lock(&speaker_flag_m);
            printf("start speaker %d\n", speaker_flag);
            speaker.StartSpeaker(speaker_flag);
            speaker_flag = 0;
            pthread_mutex_unlock(&speaker_flag_m);
        }
    }
	return NULL;
}

void *tStartStopStreaming (void *arg)
{
    while(1)
    {
        if(streaming_flag == 1 && stream.StreamStatus() == false)
        {
            stream.StartStreaming();
            printf("start streaming\n");
        }
        else if(streaming_flag == 0 && stream.StreamStatus() == true)
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
        if(motor_flag == 1 && motor.MotorStatus() == false)
        {
            motor.StartMotor();
            printf("start motor\n");
        }
        else if(motor_flag == 0 && motor.MotorStatus() == true)
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
        pthread_mutex_lock(&firebase_m);
        int streaming_flag_aux = dbhandler.GetStreamingFlag();
        int speaker_flag_aux = dbhandler.GetSpeakerFlag();
        int change_wifi_flag_aux = dbhandler.GetChangeWifiFlag();
        pthread_mutex_unlock(&firebase_m);

        streaming_flag = streaming_flag_aux;
        //printf("streaming_f: %d\n", streaming_flag);
        pthread_mutex_lock(&speaker_flag_m);
        speaker_flag = speaker_flag_aux;
        //printf("speaker_f: %d\n", speaker_flag);
        pthread_mutex_unlock(&speaker_flag_m);
        change_wifi_flag = change_wifi_flag_aux;
        //printf("change_wifi_f: %d\n", change_wifi_flag);


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
            weight_aux = scheduling.GetWeight() + weight_var; //loads weight_aux with desired weight
            printf("weight: %d\n",weight_aux);
        
            pthread_mutex_lock(&motor_flag_m); 
            motor_flag = 1;
            printf("motor_flag = true\n"); 
            pthread_cond_wait(&weight_cond, &motor_flag_m); //waits for the desired weight
            motor_flag = 0;
            printf("motor_flag = false\n");
            pthread_mutex_unlock(&motor_flag_m);
        }
        sleep(1);
    }
	return NULL;
}

void *tCheckWeight (void *arg)
{
	while(1)
    {  
        if(motor_flag == true)
        {
            pthread_mutex_lock(&weight_m);
            weight_var++;
            pthread_mutex_unlock(&weight_m);
        }
        //printf("%d\n", weight_var);
        //weight_var = sensor.ReadWeightSample();
        if((weight_var >= weight_aux) && motor_flag == true)
        {
            pthread_cond_signal(&weight_cond); //unBlocks FeedingStatus if weight achieved
        }
        sleep(1);
    }
	return NULL;
}

int main (int argc, char *argv[])
{
	int thread_policy;
    int status;
	pthread_t ChangeWifiID, StartStopSpeakerID, StartStopStreamingID, StartStopMotorID, FlagCheckID, FeedingStatusID, CheckWeightID;

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
    if(!dbhandler.LoginUser(userId))
    {
        printf("login error");
        return 0;
    }

    //system("./dScheduledTimes");
    system("./daemonrasp");

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