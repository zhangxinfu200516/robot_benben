#include "motor.h"

// ==============================
// 电机硬件配置
// ==============================
#define MAX_MOTOR_DUTY    40      // 最大占空比

// 电机方向控制GPIO
GPIO Motor_LDirection(Motor_LDirection_pin);
GPIO Motor_RDirection(Motor_RDirection_pin);

// 电机PWM控制
PWM_GTIM Motor_L_speed(Motor_Rspeed_pin, 0b11, 3, Motor_period, 0);
PWM_GTIM Motor_R_speed(Motor_Lspeed_pin, 0b11, 2, Motor_period, 0);

// ==============================
// 电机占空比设置
// direction: 0=左轮 1=右轮
// ==============================
void motor_set_duty(float set_duty, bool direction)
{
    // 占空比限幅
    if(set_duty > MAX_MOTOR_DUTY) set_duty = MAX_MOTOR_DUTY;
    if(set_duty < 0.0f) set_duty = 0.0f;

    // 计算并设置占空比
    uint32_t duty_cycle = set_duty * (Motor_period / 100);
    if(direction == 0)
    {
        Motor_L_speed.setDutyCycle(duty_cycle);
    }
    else
    {
        Motor_R_speed.setDutyCycle(duty_cycle);
    }
}

// ==============================
// 电机初始化
// ==============================
void motor_init(void)
{
    // 方向引脚初始化（默认前进）
    Motor_LDirection.setDirection("out");
    Motor_RDirection.setDirection("out");
    Motor_LDirection.setValue(1);
    Motor_RDirection.setValue(1);

    // PWM初始化（使能+默认占空比）
    Motor_L_speed.enable();
    Motor_R_speed.enable();
    motor_set_duty(15, 0);
    motor_set_duty(15, 1);
}

// ==============================
// 电机前进
// ==============================
void motor_forward(void)
{
    Motor_LDirection.setValue(1);
    Motor_RDirection.setValue(1);
}

// ==============================
// 电机后退
// ==============================
void motor_backward(void)
{
    Motor_LDirection.setValue(0);
    Motor_RDirection.setValue(0);
}

// ==============================
// 电机停止
// ==============================
void motor_stop(void)
{   
    //不要设0
    // motor_set_duty(0, 0);
    // motor_set_duty(0, 1);
    motor_set_duty(1, 0);
    motor_set_duty(1, 1);
}