#ifndef DHANDLER_H
#define DHANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include "/home/hugo/buildroot/buildroot-2021.08/output/host/include/python3.9/Python.h"
//#include "/usr/include/python3.10/Python.h"

#define ERROR 7

class CDatabaseHandler {
    public:
        ~CDatabaseHandler();
        int FirebaseInit();
        int LoginUser(char* userId); //char* email, char* password);
        bool CompareTimes(int year, int month, int day, int hour, int minute, int *weight);
        int GetStreamingFlag();
        int GetSpeakerFlag();
        int GetChangeWifiFlag();
        bool DownloadAudio(int index);
        bool ResetAudioValue();
    
    private:
        PyObject *pName, *pModule, *pDict;
        PyObject *pFuncFirebaseInit, *pFuncLoginUser, *pFuncGetStreamingFlag, *pFuncGetSpeakerFlag, *pFuncGetChangeWifiFlag, *pFuncGetSchedulingTimes, *pFuncReturnWeight, *pFuncDownloadAudio, *pFuncResetAudioValue;
        PyObject *pReturn;
};

#endif