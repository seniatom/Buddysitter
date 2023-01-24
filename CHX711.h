#ifndef CHX711_H
#define CHX711_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h> 
#include <sys/wait.h>

class CHX711
{
public:
    ~CHX711();
    void InitHX771();
    void RemHX771();
    int ReadWeightSample();
};

#endif