#include "CHX711.h"


CHX711::CHX711()
{
    printf("\n\nInserting Device Driver HX711...\n");
    system("insmod hx711.ko");
    TareWeightSensor();
    printf("\n\nColocar peso de 1kg (10s)...\n");
    sleep(10);
    CalibrateWeightSensor();
}

CHX711::~CHX711()
{
    printf("\n\nRemoving Device Driver HX711...\n");
    system("rmmod hx711");  
}

void CHX711::TareWeightSensor()
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

void CHX711::CalibrateWeightSensor()
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

int CHX711::ReadWeightSample()
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
