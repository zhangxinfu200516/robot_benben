#include "timer.h"
#include "config.h"
#include "machine.h"

int timeStamp=0;
// ==============================
// 定时器1回调：时间戳更新（1ms）
// ==============================
void pit1_callback() {
    timeStamp = (timeStamp + 1) % 1000000;
}

