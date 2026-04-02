#ifndef SENSOR_H
#define SENSOR_H


#include "loongson.h"


#define R_encoder_PWMpin    64
#define L_encoder_PWMpin    67
#define R_encoder_GPIOpin   75
#define L_encoder_GPIOpin   72



extern double R_speed;
extern double L_speed;


void sensor_encoder_get(void);


#endif