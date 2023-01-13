#ifndef MOTOR_H
#define MOTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/wait.h>

class CMotor
{
public:
    // ~CMotor();
    CMotor()
    {
        Motor_Status = false;
    }
    void InitMotor();
    void RemMotor();
    void StartMotor();
    void StopMotor();
    bool MotorStatus();

private:
    bool Motor_Status = false;
};

#endif
