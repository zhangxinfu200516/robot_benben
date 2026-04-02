#include "gl_headfile.h"
// #include "timer.h"

enum barrier_type_e barrier_type = BARRIER_NONE;
enum s_type_e s_type             = S_NONE;
enum shield_type_e shield_type   = SHIELD_NONE;

extern int beef;
extern int time_barrier;

void CheckBarrier() {

    if(timeStamp-time_begin>=500) 
        time_begin = 0;
    int w1_cnt = 0, w2_cnt = 0, flag = 0;
    for(int w = 0; w < MT9V03X_W; ++w) 
        w1_cnt += (mt9v03x_image_copy[150][w] == 255);
    for(int w = 0; w < MT9V03X_W; ++w) 
        w2_cnt += (mt9v03x_image_copy[170][w] == 255);
    if(w1_cnt > 300 && w2_cnt > 300) {
        barrier_type = BARRIER_RUNNING;
        cross_type = CROSS_NONE;
        zebra_type = ZEBRA_NONE;
        time_barrier = timeStamp;
        zbr_flag = 0;
        // wonderEchoSend(0xff,0x06);
        puts("Yes");
    }
}

void RunBarrier() {
    if(timeStamp-time_barrier<=200) {
        if(latest_range_mm.load() >= 200 && latest_range_mm.load() <= 800) {
            int row_start = 160;
            int row_end = 220;
            Mat roi = img_bin.rowRange(row_start, row_end);
            Mat left_half = roi.colRange(0, IMAGE_W / 2);
            Mat right_half = roi.colRange(IMAGE_W / 2, IMAGE_W);
            int black_left = cv::countNonZero(left_half == 0);
            int black_right = cv::countNonZero(right_half == 0);
            if (black_left > black_right) {
                pure_angle = -15;
            }
            else if (black_left < black_right) {
                pure_angle = 15;
            }
            else {
                pure_angle = 0;
            }
        }
        else {
            aim_distance_select();
            MidLineTrack();
            beef = 1;
            pure_angle = (abs(pure_angle) < 1) ? 10 : pure_angle;
        }
    }
    else {
        // wonderEchoSend(0xff,0x06);
        beef = 0;
        barrier_type = BARRIER_NONE;
    }
}