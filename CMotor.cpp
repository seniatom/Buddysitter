#include "CMotor.h"

CMotor::~CMotor()
{
    
}

void CMotor::InitMotor()
{

}

void CMotor::RemMotor()
{
    
}

void CMotor::StartMotor()
{
    if(Motor_Status == false)
    {
        //start motor dd
    }
    Motor_Status = true;
}

void CMotor::StopMotor()
{
    //stop motor dd
    Motor_Status = false;
}

bool CMotor::MotorStatus()
{
    return Motor_Status;
}

