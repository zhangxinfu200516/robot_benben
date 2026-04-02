#include "config.h"

// ====================== 全局配置实例 ======================
Config g_config;

// ====================== 运行时全局变量定义 ======================
std::atomic<bool> system_running{true};
std::atomic<bool> stop_flag{false};
std::atomic<bool> emergency_stop{false};
bool stopflag = false;
bool zebra_flag = false;
float pure_last = 0.0f;
bool start_low_flag = true;
int beef = 0;
int time_barrier = 0;

float angle = 0.0f;
float angle_last = 0.0f;

double R_speed = 0.0;
double L_speed = 0.0;
float speed_now = 0.0f;


IncrementalPID PID_L(0.002f, 0.05f, 0.0f);
IncrementalPID PID_R(0.002f, 0.05f, 0.0f);

// ====================== 配置加载函数（不动） ======================
int config_load(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[配置] 未找到 config.txt，使用默认参数\n");
        g_config.pid_left_kp = 0.002f;
        g_config.pid_left_ki = 0.05f;
        g_config.pid_left_kd = 0.0f;
        g_config.pid_right_kp = 0.002f;
        g_config.pid_right_ki = 0.05f;
        g_config.pid_right_kd = 0.0f;
        PID_L.setPID(g_config.pid_left_kp, g_config.pid_left_ki, g_config.pid_left_kd);
        PID_R.setPID(g_config.pid_right_kp, g_config.pid_right_ki, g_config.pid_right_kd);

        g_config.base_speed = 30.0f;
        g_config.max_motor_duty = 40;
        g_config.angle_threshold = 1.5f;
        servo_set_mid =90;

        g_config.steer_kp = 0.3f;
        g_config.steer_kd = 0.05f;
        g_config.steer_ratio = 0.7f;
        g_config.wheel_base = 15.0f;
        g_config.wheel_track = 12.0f;
        return -1;
    }

    printf("[配置] 成功加载 config.txt\n");

    g_config.pid_left_kp      = config_get_float(fp, "PID_L_KP");
    g_config.pid_left_ki      = config_get_float(fp, "PID_L_KI");
    g_config.pid_left_kd      = config_get_float(fp, "PID_L_KD");

    g_config.pid_right_kp     = config_get_float(fp, "PID_R_KP");
    g_config.pid_right_ki     = config_get_float(fp, "PID_R_KI");
    g_config.pid_right_kd     = config_get_float(fp, "PID_R_KD");

    PID_L.setPID(g_config.pid_left_kp, g_config.pid_left_ki, g_config.pid_left_kd);
    PID_R.setPID(g_config.pid_right_kp, g_config.pid_right_ki, g_config.pid_right_kd);

    g_config.base_speed       = config_get_float(fp, "BASE_SPEED");
    g_config.max_motor_duty   = config_get_int(fp, "MAX_MOTOR_DUTY");
    g_config.angle_threshold  = config_get_float(fp, "ANGLE_THRESHOLD");
    servo_set_mid    = config_get_float(fp, "servo_set_mid");

    g_config.steer_kp         = config_get_float(fp, "STEER_KP");
    g_config.steer_kd         = config_get_float(fp, "STEER_KD");
    g_config.steer_ratio      = config_get_float(fp, "STEER_RATIO");
    g_config.wheel_base       = config_get_float(fp, "WHEEL_BASE");
    g_config.wheel_track      = config_get_float(fp, "WHEEL_TRACK");

    fclose(fp);
    return 0;
}

float config_get_float(FILE* fp, const char* key) {
    rewind(fp);
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#') continue;
        if (strstr(line, key) != NULL) {
            char* pos = strchr(line, '=');
            if (pos) return atof(pos + 1);
        }
    }
    return 0.0f;
}

int config_get_int(FILE* fp, const char* key) {
    return (int)config_get_float(fp, key);
}