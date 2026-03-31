#include "keyboard_control.h"
#include "config.h"
#include "control.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

// 全局变量，用于保存终端状态
static int old_flags_global;
static struct termios old_tio_global;

// 恢复终端函数
static void restore_terminal_global()
{
    // 恢复 stdin 阻塞模式
    fcntl(STDIN_FILENO, F_SETFL, old_flags_global);

    // 恢复终端模式
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio_global);
}

void keyboard_task()
{
    // 保存原始终端设置
    tcgetattr(STDIN_FILENO, &old_tio_global);
    struct termios new_tio = old_tio_global;

    // 无缓冲、不回显，但保留 Ctrl+C
    new_tio.c_lflag &= ~(ICANON | ECHO);
    new_tio.c_lflag |= ISIG;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    // 设置 stdin 非阻塞
    old_flags_global = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, old_flags_global | O_NONBLOCK);

    // 注册退出时恢复终端
    atexit(restore_terminal_global);

    char key;
    bool last_stop = false;

    while (system_running && !stop_flag)
    {
        // 非阻塞读取按键，没有就跳过
        ssize_t n = read(STDIN_FILENO, &key, 1);

        if (n == 1)
        {
            // 按下 1 或 s → 急停
            if (key == '1' || key == 's' || key == 'S')
            {
                if (!last_stop)
                {
                    emergency_stop = true;
                    motor_stop();
                    printf("\n[急停] 速度=0\n");
                    last_stop = true;
                }
            }
            // 按下 r → 恢复
            else if (key == 'r' || key == 'R')
            {
                emergency_stop = false;
                printf("\n[恢复] 运行\n");
                last_stop = false;
            }
            // 按下 q → 退出
            else if (key == 'q' || key == 'Q')
            {
                emergency_stop = true;
                motor_stop();
                system_running = false;
                stop_flag = true;
                printf("\n退出程序\n");
                break;
            }
        }

        // 不卡线程
        this_thread::sleep_for(chrono::milliseconds(20));
    }

    restore_terminal_global();
}