#include "CMotor.h"

int servo0;

CMotor::~CMotor()
{
    close(servo0);
    // system("rmmod servo");  
}

CMotor::CMotor()
{
    printf("\n\nInserting Device Driver...\n");
    system("insmod servo.ko");

    servo0 = open("/dev/servo0", O_WRONLY);
}

int CMotor::GetDigits(int num)
{
    int count = 0;

    if(num == 0)
    {
        count = 1;
    }
    else
    {
        printf ("couting: %d %d\n", count, num);
        while(num > 0)
        {
            num = num / 10;
            count++;
        }
    }
    printf ("couting: %d %d\n", count, num);
    return count;
}

void CMotor::StartMotor(int i)
{
    char buffer [sizeof(int)];
    int count = this->GetDigits(i);

    printf ("count: %d\n", count);

    int buffersize = count + 1;
    snprintf(buffer, buffersize , "%d", i);

    printf ("decimal: %s %d\n",buffer, count);
    write(servo0, &buffer, count);
    
    Motor_Status = true;
}

void CMotor::StopMotor()
{
    write(servo0, 0, 1);

    Motor_Status = false;
}

bool CMotor::MotorStatus()
{
    return Motor_Status;
}

int main(void)
{
    CMotor motor;
    printf("\n\ Writting 0...\n");
    motor.StartMotor(0);
    sleep(5);
    printf("\n\ Writting 512...\n");
    motor.StartMotor(512);
    sleep(5);
    printf("\n\ Writting 1024...\n");
    motor.StartMotor(1024);
    sleep(10);
    motor.StopMotor();
   

    return 0;
}