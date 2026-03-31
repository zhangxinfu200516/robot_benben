#ifndef MACHINE_H
#define MACHINE_H

#include "loongson.h"

#include "motor.h"
#include "sensor.h"
#include "servo.h"
#include "PID.h"
#include "gl_headfile.h"


extern float BASE_SPEED;
extern float speed_now;
extern float speed_Lrotate;
extern float speed_Rrotate;

void machine_speed_change(float duty_cycle_up);
float machine_angPD(float pure_angle);
void Lspeed_close_loop(float setpoint);
void Rspeed_close_loop(float setpoint);
void machine_zebra_stop(void);
void ackermann_calc_speed(float steer_angle);



#endif