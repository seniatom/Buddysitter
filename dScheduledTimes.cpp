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

#include "CScheduling.cpp"
#include "CDatabaseHandler.cpp"

using namespace std;

struct{
    int year;
    int month;
    int day;
    int hour;
    int minute;
} CurrentTime;

int weight = 0;

char userId[] = "YW7b3hkFsaTo7qfU5gZ5tyHJ85v2";
char daemon_email[] = "daemon.admin@buddysitter.com";
char daemon_password[] = "123456";

CScheduling scheduling;
CDatabaseHandler dbhandler;

void signal_handler(int sig) {
switch(sig) {
    case SIGHUP:
        syslog(LOG_INFO,"hangup signal catched");
    break;
    case SIGTERM:
        syslog(LOG_INFO,"terminate signal catched");
        exit(0);
    case SIGALRM:
        syslog(LOG_INFO,"timer overflow");
		//Get current time and compare to times in database//
        scheduling.GetCurrentTime(&CurrentTime.year,&CurrentTime.month,&CurrentTime.day,&CurrentTime.hour,&CurrentTime.minute); 
        if(dbhandler.CompareTimes(CurrentTime.year,CurrentTime.month,CurrentTime.day,CurrentTime.hour,CurrentTime.minute,&weight) == true)
        {
            scheduling.MsgQueueSend(weight); //Send weight to msg queue
		    sleep(60);	
        }
        alarm(1);
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

	if (sid < 0) { 
		perror("setsid");
		exit(EXIT_FAILURE);
	}
	// make '/' the root directory
	if (chdir("/") < 0) { 
		perror("chdir");
		exit(EXIT_FAILURE);
	}
	umask(0);

	//close standard file descriptor//
	close(STDIN_FILENO);  
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

    signal(SIGALRM,signal_handler);
	signal(SIGHUP,signal_handler);
	signal(SIGTERM,signal_handler);

    //setenv("PYTHONPATH","/etc/python_code", 1);

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
    alarm(1);

	while (1) {
        pause();
	}
exit(EXIT_SUCCESS);		
}