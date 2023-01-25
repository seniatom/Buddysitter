#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <syslog.h>

#include "../CScheduling.h"

using namespace std;

struct{
    int year;
    int month;
    int day;
    int hour;
    int minute;
} CurrentTime;

int feedingtime = 15; //get this from file / database

CScheduling scheduling;

void signal_handler(int sig) {
switch(sig) {
    case SIGHUP:
        syslog(LOG_INFO,"hangup signal catched");
    break;
    case SIGTERM:
        syslog(LOG_INFO,"terminate signal catched");
        exit(0);
    case SIGALRM:
        syslog(LOG_INFO,"timer moment");
        scheduling.GetCurrentTime(&CurrentTime.year,&CurrentTime.month,&CurrentTime.day,&CurrentTime.hour,&CurrentTime.minute);
        if(scheduling.compareTimes(CurrentTime.year,CurrentTime.month,CurrentTime.day,CurrentTime.hour,CurrentTime.minute,&feedingtime) == true)
        {
            scheduling.MsgQueueSend();
        }
        alarm(20);
    break;
    }
}

int main(int argc, char *argv[]){
	pid_t pid, sid;

	pid = fork(); // create a new process

	if (pid < 0) { // on error exit
		perror("fork");
	    exit(EXIT_FAILURE);
	}

	if (pid > 0){  
		printf("Deamon PID: %d\n", pid);	
		exit(EXIT_SUCCESS); // parent process (exit)
	}
	sid = setsid(); // create a new session

	if (sid < 0) { // on error exit
		perror("setsid");
		exit(EXIT_FAILURE);
	}
	// make '/' the root directory
	if (chdir("/") < 0) { // on error exit
		perror("chdir");
		exit(EXIT_FAILURE);
	}
	umask(0);
	close(STDIN_FILENO);  // close standard input file descriptor
	close(STDOUT_FILENO); // close standard output file descriptor
	close(STDERR_FILENO); // close standard error file descriptor

    signal(SIGALRM,signal_handler);
	signal(SIGHUP,signal_handler);
	signal(SIGTERM,signal_handler);
	
    alarm(20);

	while (1) {
        pause();
	}
exit(EXIT_SUCCESS);		
}