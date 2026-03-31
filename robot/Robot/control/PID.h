#ifndef PID_H
#define PID_H


class IncrementalPID {//增量式PID
public:
    // 构造函数，初始化PID参数
    IncrementalPID(float Kp, float Ki, float Kd);

    // 计算控制量增量
    float calculate(bool direction,float setpoint, float measured_value);

    // 重置控制器状态
    void reset();

    void setPID(float new_Kp, float new_Ki, float new_Kd);

//private:
    float Kp, Ki, Kd;       // PID参数
    float prev_error;       // 上一次的误差
    float prev2_error;      // 上上次的误差
    float P_part;           //
    float I_part;
    float D_part;
    float output;           // 当前输出值
};



extern IncrementalPID   PID_L;
extern IncrementalPID   PID_R;






#endif