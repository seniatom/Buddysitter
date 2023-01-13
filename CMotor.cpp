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
        if (!bcm2835_init())
	        return; //return 1
        
        bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);
        
        bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_32);
        bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
        bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
        
        int data = 512;
        bcm2835_pwm_set_data(PWM_CHANNEL, data);
	    bcm2835_delay(1);
        
    }
    Motor_Status = true;
}

void CMotor::StopMotor()
{
    //stop motor dd
    int data = 0;
    bcm2835_pwm_set_data(PWM_CHANNEL, data);
	bcm2835_delay(1);
    bcm2835_close();
    
    Motor_Status = false;
}

bool CMotor::MotorStatus()
{
    return Motor_Status;
}

