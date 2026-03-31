#include "zf_common_headfile.h"
#include "loongson.h"
#include "gl_headfile.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <pthread.h>
#include <sched.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include "control.h"

using namespace std;
using namespace cv;

// ==========================
// 退出清理
// ==========================
void sigint_handler(int signum)
{
    system_running = false;
    stop_flag = true;
    motor_stop();
    printf("程序安全退出\n");
    exit(0);
}

void cleanup()
{
    motor_stop();
    printf("资源已释放\n");
}

// ==========================
// 视觉处理（预留接口）
// ==========================
int vision_process(Mat &frame)
{
    int dx = 0;
    // 视觉寻线逻辑：计算偏移量dx
    return dx;
}

// ==========================
// 运动控制线程（CPU1 独占）
// ==========================
void motion_control_task()
{
    // 线程CPU亲和性设置
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    // 设置线程调度优先级
    struct sched_param param{};
    param.sched_priority = 99;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    // 1ms定时器创建
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec its{};
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 1 * 1000000; // 1ms
    its.it_value = its.it_interval;
    timerfd_settime(tfd, 0, &its, NULL);

    uint64_t exp;
    int tick = 0;

    while(system_running && !stop_flag)
    {
        read(tfd, &exp, sizeof(exp));
        tick++;

        // 每5ms更新一次编码器和转向控制
        if(tick % 5 == 0)
        {
            sensor_encoder_get();                  // 更新编码器速度
            Lspeed_close_loop(25);
            Rspeed_close_loop(25);
            // angle = machine_angPD(pure_angle);     // 计算PD转向角
            
            // ackermann_calc_speed(angle);           // 阿克曼差速计算
            
            // // 角度变化超过1.5°时更新舵机
            // if(fabs(angle_last - angle) >= 1.5f)
            // {
            //     servo_set(angle);           
            //     angle_last = angle;
            // }
        }
    this_thread::sleep_for(chrono::microseconds(50));
    }
    
    close(tfd);
    motor_stop();
}

// ==========================
// 视觉线程（CPU0）
// ==========================
void vision_task(VideoCapture &cap)
{
    Mat frame;
    while(system_running && !stop_flag)
    {
        cap >> frame;
        if(frame.empty()) break;

        int dx = vision_process(frame);
        // vision_offset.store(dx); // 预留：视觉偏移量传递

        this_thread::sleep_for(chrono::milliseconds(30));
    }
}
// void vision_task(VideoCapture &cap)
// {
//     Mat frame;
//     int frame_count = 0;

//     while(system_running && !stop_flag)
//     {
//         cap >> frame;
//         if(frame.empty()) break;

//         // 每隔 30 帧保存一次图片
//         if (frame_count % 30 == 0)
//         {
//             imwrite("/home/root/robot/vision.jpg", frame);
//             printf("图片已保存到 vision.jpg\n");
//         }
//         frame_count++;

//         int dx = vision_process(frame);

//         this_thread::sleep_for(chrono::milliseconds(30));
//     }
// }

// ==========================
// 主函数（初始化+线程管理）
// ==========================
int main(int, char**)
{
    config_load("config.txt");
    // 注册退出清理函数和信号处理
    atexit(cleanup);
    signal(SIGINT, sigint_handler);

    // ======================
    // 第一步：硬件初始化
    // ======================
    wonderEchoInit();
    servo_init();
    motor_init();
    
    // 摄像头初始化
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "摄像头打开失败" << endl;
        return -1;
    }
    cap.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M','J','P','G'));
    cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CAP_PROP_FRAME_HEIGHT, 240);
    

    // ======================
    // 第二步：启动线程
    // ===================s===
    thread vision_th(vision_task, ref(cap));
    thread motion_th(motion_control_task);
    // thread keyboard_th(keyboard_task);

    // ======================
    // 主线程保持运行
    // ======================
    while(system_running && !stop_flag)
    {
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    // 等待线程结束
    vision_th.join();
    motion_th.join();
    // keyboard_th.join();  

    return 0;
}