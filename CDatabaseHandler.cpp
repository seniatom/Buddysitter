#include "CDatabaseHandler.h"

CDatabaseHandler::~CDatabaseHandler()
{

}

int CDatabaseHandler::FirebaseInit()
{
    //setenv("PYTHONPATH","/home/hugo/Visual_Studio_Code/python", 1);
    Py_InitializeEx(0);

    pName = PyUnicode_FromString((char*)"DatabaseHandler");

    pModule = PyImport_Import(pName);

    if(pModule == NULL)
    {
        printf("No module with that name");
        return -ERROR;
    }

    pDict = PyModule_GetDict(pModule);

    Py_DECREF(pName);
    Py_DECREF(pModule);

    pFuncFirebaseInit = PyDict_GetItemString(pDict, (char*)"FirebaseInit");

    if (!PyCallable_Check(pFuncFirebaseInit))
    {
        PyErr_Print();
        return 0;
    }

    pFuncLoginUser = PyDict_GetItemString(pDict, (char*)"LoginUser");

    if (!PyCallable_Check(pFuncLoginUser))
    {
        PyErr_Print();
        return 0;
    }

    pFuncGetStreamingFlag = PyDict_GetItemString(pDict, (char*)"GetStreamingFlag");

    if (!PyCallable_Check(pFuncGetStreamingFlag))
    {
        PyErr_Print();
        return 0;
    }

    pFuncGetSpeakerFlag = PyDict_GetItemString(pDict, (char*)"GetSpeakerFlag");

    if (!PyCallable_Check(pFuncGetSpeakerFlag))
    {
        PyErr_Print();
        return 0;
    }

    pFuncGetChangeWifiFlag = PyDict_GetItemString(pDict, (char*)"GetChangeWifiFlag");

    if (!PyCallable_Check(pFuncGetChangeWifiFlag))
    {
        PyErr_Print();
        return 0;
    }

    pFuncGetSchedulingTimes = PyDict_GetItemString(pDict, (char*)"GetSchedulingTimes");

    if (!PyCallable_Check(pFuncGetSchedulingTimes))
    {
        PyErr_Print();
        return 0;
    }

    pFuncDownloadAudio = PyDict_GetItemString(pDict, (char*)"DownloadAudio");

    if (!PyCallable_Check(pFuncDownloadAudio))
    {
        PyErr_Print();
        return 0;
    }

    pFuncResetAudioValue = PyDict_GetItemString(pDict, (char*)"ResetAudioValue");

    if (!PyCallable_Check(pFuncResetAudioValue))
    {
        PyErr_Print();
        return 0;
    }

    pReturn = PyObject_CallFunction(pFuncFirebaseInit, NULL);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    return 1;
}

int CDatabaseHandler::LoginUser(char* userId) //char* email, char* password)
{
    pReturn = PyObject_CallFunction(pFuncLoginUser, "s", userId); //ss", userId, email, password);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }
    Py_DECREF(pReturn);
    printf("Login Successfull\n");

    return 1;
}

bool CDatabaseHandler::CompareTimes(int year, int month, int day, int hour, int minute, int *weight)
{
    pReturn = PyObject_CallFunction(pFuncGetSchedulingTimes, "iiiii", year, month, day, hour, minute);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int ret = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    if(ret == 0)
    {
        return false;
    }else{
        *weight = ret;
        return true;
    }
}

int CDatabaseHandler::GetStreamingFlag()
{
    pReturn = PyObject_CallFunction(pFuncGetStreamingFlag, NULL);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int flag = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    return flag;
}

int CDatabaseHandler::GetSpeakerFlag()
{
    pReturn = PyObject_CallFunction(pFuncGetSpeakerFlag, NULL);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int ret = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    return ret;
}

int CDatabaseHandler::GetChangeWifiFlag()
{
    pReturn = PyObject_CallFunction(pFuncGetChangeWifiFlag, NULL);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int flag = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    return flag;
}

bool CDatabaseHandler::DownloadAudio(int index)
{
    char number[2];
    sprintf(number, "%d", index);
    pReturn = PyObject_CallFunction(pFuncDownloadAudio, "s", number);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int ret = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    return ret;
}

bool CDatabaseHandler::ResetAudioValue()
{
    pReturn = PyObject_CallFunction(pFuncResetAudioValue, NULL);
    if(pReturn == NULL)
    {
        PyErr_Print();
        return 0;
    }

    int ret = (int)PyLong_AsLong(pReturn);

    Py_XDECREF(pReturn);

    return ret;
}

