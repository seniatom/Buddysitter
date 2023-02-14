#include "CWeightSensor.h"


CWeightSensor::CWeightSensor()
{
    printf("\n\nInserting Device Driver HX711...\n");
    system("insmod hx711.ko");
    TareWeightSensor();
    printf("\n\nColocar peso de 1kg (10s)...\n");
    sleep(10);
    CalibrateWeightSensor();
}

CWeightSensor::~CWeightSensor()
{
    printf("\n\nRemoving Device Driver HX711...\n");
    system("rmmod hx711");  
}

void CWeightSensor::TareWeightSensor()
{
    scale0 = open("/dev/hx7110", O_RDONLY);

    char sample[10];
    const int loops = 20;
    zero = 0;
    for(int i = 0; i < loops; i++)
    {
        read(scale0, &sample, 10);

        zero = zero + atoi(sample);
        printf("\nzero -> %d    sample %d -> %d", zero, i, atoi(sample));
    }
    zero = zero / loops; 

    close(scale0);
    printf("\n media -> %d\n", zero);  
}

void CWeightSensor::CalibrateWeightSensor()
{ 
    scale0 = open("/dev/hx7110", O_RDONLY);

    char sample[10];
    int reading = 0;
    const int loops = 20;

    for(int i = 0; i < loops; i++)
    {
        read(scale0, &sample, 10);

        reading = reading + atoi(sample);
    }
    reading = reading / loops; 

    calibration_value = zero - reading;
    printf("\n calibration_value -> %d    zero:%d    read:%d \n", calibration_value, zero, atoi(sample));

    close(scale0);
}

int CWeightSensor::readWeightSensor()
{
    scale0 = open("/dev/hx7110", O_RDONLY);

    int weight_read; 
    char sample[10];

    read(scale0, &sample, 10);
    weight_read = zero - atoi(sample);

    WeightSensor_Status = (weight_read*1000)/ calibration_value;

    printf("\n\n weight read(g) -> %d ", weight_read);
    printf(" calibration(g) -> %d ", calibration_value);
    printf(" weight (g) \n\n-> %d \n", WeightSensor_Status);

    close(scale0);

    return WeightSensor_Status;
}

// int main(void)
// {
//     CWeightSensor sensor;
//     sensor.TareWeightSensor();
//     printf("\n Calibrate weight...\n");
//     sleep(10);
//     sensor.CalibrateWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
//     sleep(5);
//     sensor.readWeightSensor();
    

//     return 0;
// }