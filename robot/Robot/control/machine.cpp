#include "machine.h"
#include <math.h>
#include "config.h"

// ==============================
// 小车物理参数配置（根据实际硬件修改）
// ==============================
#define WHEEL_BASE       0.0f    // 轴距 cm (前后轮中心距)
#define WHEEL_TRACK      0.0f    // 轮距 cm (左右后轮中心距)
#define MAX_STEER_ANGLE  0.0f    // 最大转向角 °
#define BASE_SPEED     0.0f    // 基础速度 cm/ss
#define STEER_RATIO      0.7f    // 转向比例系数

// 转向PD控制器参数
#define STEER_KP         0.0f    // 比例系数
#define STEER_KD         0.0f    // 微分系数

// 模式控制参数
#define BARRIER_KP       0.0f    // 避障比例系数
#define BARRIER_OUT_KP   0.0f    // 避障退出比例系数
#define CROSS_BEGIN_KP   0.0f    // 十字起始比例系数
#define CROSS_KP         0.0f    // 十字比例系数

// ==============================
// 静态变量（仅本文件可见）
// ==============================
static float target_speed = BASE_SPEED;  // 目标速度
static float last_angle = 0.0f;          // 上次计算角度

// ==============================
// 速度调整接口
// ==============================
void machine_speed_change(float delta)
{
    target_speed = BASE_SPEED + delta;
}

// ==============================
// 核心：阿克曼差速计算（运动学公式）
// ==============================
void ackermann_calc_speed(float steer_angle)
{
    // 角度转弧度
    float delta_rad = steer_angle * M_PI / 180.0f;
    float tan_delta = tan(delta_rad);

    // 标准阿克曼差速比计算
    float ratio = (WHEEL_TRACK * tan_delta) / (2.0f * WHEEL_BASE);

    // 计算左右轮目标速度
    float v_left  = target_speed * (1.0f - ratio * STEER_RATIO);
    float v_right = target_speed * (1.0f + ratio * STEER_RATIO);

    // 输出到速度闭环控制
    Lspeed_close_loop(v_left);
    Rspeed_close_loop(v_right);
}

// ==============================
// 转向PD控制器
// ==============================
float machine_angPD(float pure_angle)
{
    float angle_out = 0.0f;
    float angle_diff = pure_angle - last_angle;

    // 避障模式处理
    if (barrier_type == 1)  // 建议替换为枚举：BARRIER_RUNNING
    {
        angle_out = (beef == 1) ? (BARRIER_OUT_KP * pure_angle) : (BARRIER_KP * pure_angle);
    }
    else
    {
        // 十字模式/普通模式处理
        if (cross_type == 0)  // 建议替换为枚举：CROSS_NONE
        {
            // 标准PD控制
            angle_out = (STEER_KP * pure_angle) + (STEER_KD * angle_diff);
        }
        else
        {
            // 十字路口模式
            angle_out = (cross_type == 1) ?  // 建议替换为枚举：CROSS_BEGIN
                (CROSS_BEGIN_KP * pure_angle + STEER_KD * angle_diff) :
                (CROSS_KP * pure_angle + STEER_KD * angle_diff);
        }
    }

    last_angle = pure_angle;
    return angle_out;
}

// ==============================
// 左轮速度闭环控制
// ==============================
void Lspeed_close_loop(float setpoint)
{
    float pwm = PID_L.calculate(0, setpoint, L_speed);
    motor_set_duty(pwm, 0);
}

// ==============================
// 右轮速度闭环控制
// ==============================
void Rspeed_close_loop(float setpoint)
{
    float pwm = PID_R.calculate(1, setpoint, R_speed);
    motor_set_duty(pwm, 1);
}

// ==============================
// 斑马线停车逻辑
// ==============================
void machine_zebra_stop(void)
{
    motor_backward();
    motor_set_duty(40, 0);
    motor_set_duty(40, 1);

    //延时

    motor_forward();
    motor_stop();
}