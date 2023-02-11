#ifndef DHANDLER_H
#define DHANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include "/home/inesvdm/buildroot/buildroot_base/buildroot-2021.08/output/host/include/python3.9/Python.h"
//#include "/usr/include/python3.10/Python.h"

#define ERROR 7

class CDatabaseHandler {
    public:
        ~CDatabaseHandler();
        int FirebaseInit();
        int LoginUser(char* userId, char* customId);
        bool CompareTimes(int year, int month, int day, int hour, int minute, int *weight);
        bool GetStreamingFlag();
        bool GetSpeakerFlag();
        bool GetChangeWifiFlag();
    
    private:
        PyObject *pName, *pModule, *pDict;
        PyObject *pFuncFirebaseInit, *pFuncLoginUser, *pFuncGetStreamingFlag, *pFuncGetSpeakerFlag, *pFuncGetChangeWifiFlag, *pFuncGetSchedulingTimes, *pFuncReturnWeight;
        PyObject *pReturn;
};

#endif