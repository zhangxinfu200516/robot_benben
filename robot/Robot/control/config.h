#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <atomic>
#include <stdbool.h>

// 枚举
#include "../image/gl_barrier.h"
#include "../image/gl_cross.h"
#include "../image/gl_zebra.h"
#include "PID.h"

// ====================== 可调参数（从 config.txt 读取）======================
typedef struct {
    // PID参数
    float pid_left_kp;
    float pid_left_ki;
    float pid_left_kd;
    float pid_right_kp;
    float pid_right_ki;
    float pid_right_kd;

    // 系统参数
    float base_speed;
    int max_motor_duty;
    float angle_threshold;

    // 转向参数
    float steer_kp;
    float steer_kd;
    float steer_ratio;

    // 车体物理参数
    float wheel_base;
    float wheel_track;
} Config;

// ====================== 运行时全局变量（原来 global 里的全部搬过来）======================
extern std::atomic<bool> system_running;
extern std::atomic<bool> stop_flag;
extern std::atomic<bool> emergency_stop;
extern bool stopflag;
extern bool zebra_flag;
extern float pure_last;
extern bool start_low_flag;
extern int beef;
extern int time_barrier;

extern float angle;
extern float angle_last;
extern float pure_angle;
extern float servo_set_mid;
extern double R_speed;
extern double L_speed;
extern float speed_now;

extern enum barrier_type_e barrier_type;
extern enum cross_type_e   cross_type;
extern enum zebra_type_e   zebra_type;

extern IncrementalPID PID_L;
extern IncrementalPID PID_R;

// ====================== 全局配置实例 ======================
extern Config g_config;

// ====================== 函数接口 ======================
int config_load(const char* filename);
float config_get_float(FILE* fp, const char* key);
int config_get_int(FILE* fp, const char* key);

#endif