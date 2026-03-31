#include "gl_headfile.h"
int time_begin=0;
extern bool start_low_flag;


enum zebra_type_e zebra_type = ZEBRA_NONE;
void CheckZebra()
{
    if (zebra_type == ZEBRA_NONE) {
        int wb_flag = 0, w_flag = 0;
        for(int w = 0; w < IMAGE_W - 1; w ++)
            if(white_bin.at<uchar>(160, w) != white_bin.at<uchar>(160, w + 1))
                wb_flag ++;
        for(int w = 0; w < IMAGE_W - 1; w ++)
            if(white_bin.at<uchar>(160, w) == 255)
                w_flag ++;
        printf("%d %d\n",wb_flag, w_flag);
        if(wb_flag >= 6 && w_flag < 200 && w_flag > 80 && !zbr_flag)
            zebra_type = ZEBRA_RUNNING;
    }
}

void RunZebra()
{
    if(zebra_type == ZEBRA_RUNNING) {
        if(time_begin==0)
        {
            wonderEchoSend(0xff,0x02);
            machine_zebra_stop();
            time_begin=timeStamp;
        }
        if(time_begin && timeStamp-time_begin>=300)
        {
            motor_forward(); 
            time_begin = 0;
            zbr_flag = 1;
            zebra_type = ZEBRA_NONE;
            return;
        }
    }
}