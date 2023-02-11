#ifndef CHX711_H
#define CHX711_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

class CHX711
{
public:
    CHX711();
    ~CHX711();
    void TareWeightSensor();
    void CalibrateWeightSensor();
    int ReadWeightSample();
    
private:
    int scale0;
    int zero;
    int calibration_value;
    int WeightSensor_Status; 
};

#endif
