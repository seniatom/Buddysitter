#ifndef DHANDLER_H
#define DHANDLER_H

#include <stdlib.h>
#include <stdio.h>


class CDatabaseHandler {
    public:
        ~CDatabaseHandler();
        int FirebaseGetTimes();
        bool GetStreamingFlag(bool *steaming_f);
        bool GetSpeakerFlag(bool *speaker_f);
        bool GetChangeWifiFlag(bool *change_wifi_f);
};

#endif