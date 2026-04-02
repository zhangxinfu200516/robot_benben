#ifndef MOTOR_H
#define MOTOR_H

#include "loongson.h"


#define Motor_LDirection_pin    12
#define Motor_RDirection_pin    13
#define Motor_Lspeed_pin        89
#define Motor_Rspeed_pin        88


#define Motor_period            10000
#define MAX_DUTY                40

void motor_set_duty(float set_duty,bool direction);
void motor_init(void);
void motor_forward(void);
void motor_backward(void);
void motor_stop(void);


#endif