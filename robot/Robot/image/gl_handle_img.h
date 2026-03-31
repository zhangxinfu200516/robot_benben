#ifndef HANDLE_IMG
#define HANDLE_IMG
#include <gl_headfile.h>



void SearchStartPoint_Left(uint8_t img_gray[], int32_t* h_start, int32_t* w_start, uint8_t threshold);
void GetLinesFilter(float pts_in[][2], int32_t pts_in_count, float pts_out[][2], int32_t kernelsize);
void GetLinesResample(float pts_in[][2], int32_t num1, float pts_out[][2], int32_t* num2, float dist);
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void GetMidLine_Left(float pts_left[][2], int32_t pts_left_count, float mid_left[][2], int32_t approx_num, float dist);
void GetMidLine_Right(float pts_right[][2], int32_t pts_right_count, float mid_right[][2], int32_t approx_num, float dist);
int is_curve(float angle[], int n, float threshold) ;

#endif