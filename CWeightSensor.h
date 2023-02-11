#ifndef WEIGHTSENSOR_H
#define WEIGHTSENSOR_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

class CWeightSensor
{
public:

    ~CWeightSensor();
    CWeightSensor();
    
    void TareWeightSensor();
    void CalibrateWeightSensor();
    float readWeightSensor();

private:
    int scale0;
    int zero;
    int calibration_value;
    float WeightSensor_Status;
};

#endif