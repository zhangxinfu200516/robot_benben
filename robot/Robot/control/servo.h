#ifndef SERVO_H
#define SERVO_H


#include "loongson.h"


#define SERVO_pin               1           //GPIO65

#define SERVO_MOTOR_PERIOD      20000000    //2*10^7
#define SERVO_MOTOR_FREQ        ((float)1000000000 / (float)SERVO_MOTOR_PERIOD)  //10^9




// ------------------ 舵机占空比计算方式 ------------------
// 
// 舵机对应的 0-180 活动角度对应 控制脉冲的 0.5ms-2.5ms 高电平
// 
// 那么不同频率下的占空比计算方式就是
// PWM_DUTY_MAX/(1000/freq)*(1+Angle/180) 在 50hz 时就是 PWM_DUTY_MAX/(1000/50)*(1+Angle/180)
// 
// 那么 100hz 下 90度的打角 即高电平时间1.5ms 计算套用为
// PWM_DUTY_MAX/(1000/100)*(1+90/180) = PWM_DUTY_MAX/10*1.5
// 
// ------------------ 舵机占空比计算方式 ------------------
#define SERVO_MOTOR_DUTY(x)         ((float)SERVO_MOTOR_PERIOD/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))



void servo_set(float angle);
void servo_init(void);




extern float servo_set_mid;




#endif