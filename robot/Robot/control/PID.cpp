#include "PID.h"
#include <cmath>

// ==============================
// 增量式PID构造函数
// ==============================
IncrementalPID::IncrementalPID(float Kp, float Ki, float Kd)
    : Kp(Kp), Ki(Ki), Kd(Kd), 
      prev_error(0.0f), prev2_error(0.0f), output(0.0f) 
{}

// ==============================
// PID计算核心函数
// direction: 0=左轮 1=右轮
// setpoint: 目标值
// measured_value: 测量值
// ==============================
float IncrementalPID::calculate(bool direction, float setpoint, float measured_value) {
    // 1. 计算当前误差
    float error = setpoint - measured_value;

    // 2. 计算PID各分量
    P_part = Kp * (error - prev_error);          // 比例项
    I_part = Ki * error;                         // 积分项
    D_part = Kd * (error - 2 * prev_error + prev2_error);  // 微分项

    // 3. 计算输出增量
    float delta_output = P_part + I_part + D_part;

    // 4. 增量限幅
    if(delta_output > 7.0f) delta_output = 7.0f;

    // 5. 低速补偿（测量值远小于目标值时）
    if(setpoint + 70 < measured_value)
    {
        delta_output *= 1.15f;
    }

    // 6. 更新输出值
    output += delta_output;

    // 7. 极低速度时强制输出固定值
    if(measured_value <= 10.0f)
    {
        output = (direction == 0) ? 20.0f : 20.0f;
    }

    // 8. 输出限幅（最大40）
    if(output >= 40.0f) output = 40.0f;

    // 9. 更新误差历史
    prev2_error = prev_error;
    prev_error = error;

    return output;
}

// ==============================
// PID状态重置
// ==============================
void IncrementalPID::reset() {
    prev_error = 0.0f;
    prev2_error = 0.0f;
    output = 0.0f;
}

void IncrementalPID::setPID(float new_Kp, float new_Ki, float new_Kd) {
    this->Kp = new_Kp;
    this->Ki = new_Ki;
    this->Kd = new_Kd;
}