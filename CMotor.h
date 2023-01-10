#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/wait.h>

class CMotor
{
public:
    ~CMotor();
    void StartMotor();
    void StopMotor();
    bool MotorStatus();

private:
    bool Motor_Status;
};