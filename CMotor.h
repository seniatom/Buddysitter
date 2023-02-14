#ifndef MOTOR_H
#define MOTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>


class CMotor
{
public:
    ~CMotor();
    CMotor();
    int GetDigits(int num);
    void StartMotor(int i = 800);
    void StopMotor();
    bool MotorStatus();

private:
    bool Motor_Status = false;
};

#endif