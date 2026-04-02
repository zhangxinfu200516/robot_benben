#include "gl_headfile.h"
#include "timer.h"

enum cross_type_e cross_type = CROSS_NONE;
bool far_Lpt0_found, far_Lpt1_found;
int32_t Both_Boder_None_Cross;
int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;
int cross_easy   = 0;
float cross_open = 1;
int time_cross;
int cross_cnt = 0;
void CheckCross()
{
    bool Xfound = Lpt0_found && Lpt1_found;
    if ((int)cross_open == 0) {
        if (cross_type == CROSS_NONE && Xfound) {
            cross_type = CROSS_BEGIN;
            cross_cnt ++;
        }
    }
    if ((int)cross_open == 1 && (timeStamp - time_cross + 1000000) % 1000000 >= 100 ) {
        if (cross_type == CROSS_NONE && Xfound && cross_easy == 0) {
            cross_type = CROSS_BEGIN;
            cross_cnt ++;
            // wonderEchoSend(0xff,0x04);
        }
        if (cross_type == CROSS_NONE && cross_easy == 1 && (((Lpt0_found || Lpt1_found) && (pts_resample_left_count <= 3 || pts_resample_right_count <= 3)) || Xfound)) {
            cross_type = CROSS_BEGIN;
            cross_cnt ++;
            // wonderEchoSend(0xff,0x04);
        }
    }
}


double farline_cnt = 0;
void RunCross()
{
    bool Xfound = Lpt0_found && Lpt1_found;
    if (cross_easy == 0) {
        if (cross_type == CROSS_BEGIN) {
            if (Lpt0_found) {
                mid_left_count = pts_resample_left_count = Lpt0_rpts0s_id;
                track_type                               = TRACK_LEFT;
                farline_cnt                              = 0;
            } else if (Lpt1_found) {
                mid_right_count = pts_resample_right_count = Lpt1_rpts1s_id;
                track_type                                 = TRACK_RIGHT;
                farline_cnt                              = 0;
            }

            if ((Xfound && (Lpt0_rpts0s_id < 0.05 / RESAMPLEDIST && Lpt1_rpts1s_id < 0.05 / RESAMPLEDIST)) || (pts_resample_left_count < 10 && pts_resample_right_count < 10)) {
                cross_type = CROSS_IN;
                time_cross = timeStamp;
            }
        }
        if (cross_type == CROSS_IN) {
            cross_farline();
            if (timeStamp - time_cross >= 40 || pts_resample_left_count > 100 || pts_resample_right_count > 100) {
                cross_type            = CROSS_NONE;
                Both_Boder_None_Cross = 0;
                cross_easy            = 1;
            }
            if (far_Lpt1_found) {
                track_type = TRACK_RIGHT;
            } else if (far_Lpt0_found) {
                track_type = TRACK_LEFT;
            } else if (Both_Boder_None_Cross > 0 && pts_resample_right_count < 5) {
                track_type = TRACK_RIGHT;
            } else if (Both_Boder_None_Cross > 0 && pts_resample_left_count < 5) {
                track_type = TRACK_LEFT;
            }
        }
    }

    if (cross_easy == 1) {
        if (cross_type == CROSS_BEGIN) {
            if (Lpt0_found) {
                mid_left_count = pts_resample_left_count = Lpt0_rpts0s_id;
                track_type                               = TRACK_LEFT;
            } else if (Lpt1_found) {
                mid_right_count = pts_resample_right_count = Lpt1_rpts1s_id;
                track_type                                 = TRACK_RIGHT;
            }
            if ((Xfound && (Lpt0_rpts0s_id < 0.05 / RESAMPLEDIST && Lpt1_rpts1s_id < 0.05 / RESAMPLEDIST)) || (pts_resample_left_count < 10 && pts_resample_right_count < 10)) {
                cross_type = CROSS_IN;
                time_cross = timeStamp;
            }
        }
        if (cross_type == CROSS_IN) {
            cross_farline();
            if (timeStamp - time_cross >= 40 || pts_resample_left_count > 100 || pts_resample_right_count > 100)  {
                cross_type            = CROSS_NONE;
                Both_Boder_None_Cross = 0;
                cross_easy            = 0;
            }
            if (far_Lpt1_found) {
                track_type = TRACK_RIGHT;
            } else if (far_Lpt0_found) {
                track_type = TRACK_LEFT;
            } else if (Both_Boder_None_Cross > 0 && pts_resample_right_count < 5) {
                track_type = TRACK_RIGHT;
            } else if (Both_Boder_None_Cross > 0 && pts_resample_left_count < 5) {
                track_type = TRACK_LEFT;
            }
        }
    }
}


void cross_farline()
{
    int w1 = IMAGE_W / 2 - BEGINW_R, h1 = 100;
    pts_far_left_count = sizeof(pts_far_left) / sizeof(pts_far_left[0]);
    for (; w1 > 0; w1--) {
        if (GET_PIX_1C(mt9v03x_image_copy[0], h1, w1 - 1) < FIX_BINTHRESHOLD) 
            break;
    }

    if (GET_PIX_1C(mt9v03x_image_copy[0], h1, w1) >= FIX_BINTHRESHOLD){
        SearchLineAdaptive_Left(mt9v03x_image_copy[0], SELFADAPT_KERNELSIZE, SELFADAPT_OFFSET, h1, w1, pts_far_left, &pts_far_left_count);
    }
    else pts_far_left_count = 0;
    // puts("Yes");

    int w2 = IMAGE_W / 2 + BEGINW_L, h2 = 100;
    pts_far_right_count = sizeof(pts_far_right) / sizeof(pts_far_right[0]);
    for (; w2 < IMAGE_W - 1; w2++) {
        if (GET_PIX_1C(mt9v03x_image_copy[0], h2, w2 + 1) < FIX_BINTHRESHOLD) 
            break;
    }

    if (GET_PIX_1C(mt9v03x_image_copy[0], h2, w2) >= FIX_BINTHRESHOLD){
        SearchLineAdaptive_Right(mt9v03x_image_copy[0], SELFADAPT_KERNELSIZE, SELFADAPT_OFFSET, h2, w2, pts_far_right, &pts_far_right_count);
    }
    else pts_far_right_count = 0;
    // puts("Yes");
    

    // Mat temp1(240, 320, CV_8UC3, Scalar(0, 0, 0));
    // // printf("%d\n", pts_far_right_count);
    // for(int i = 0; i < pts_far_right_count - 1; ++i)
    // {
    //     if(pts_far_right[i][0] >= 0 && pts_far_right[i][1] >= 0 && pts_far_right[i][0] < 240 && pts_far_right[i][1] < 320 && pts_far_right[i + 1][0] >= 0 && pts_far_right[i + 1][1] >= 0 && pts_far_right[i + 1][0] < 240 && pts_far_right[i + 1][1] < 320)
    //     {
    //         Point startd((int)pts_far_right[i][1], (int)pts_far_right[i][0]), endd((int)pts_far_right[i + 1][1], (int)pts_far_right[i + 1][0]);
    //         line(temp1, startd, endd, Scalar(0, 255, 0), 2, LINE_AA);
    //     }
    // }
    // for(int i = 0; i < pts_far_left_count - 1; ++i)
    // {
    //     if(pts_far_left[i][0] >= 0 && pts_far_left[i][1] >= 0 && pts_far_left[i][0] < 240 && pts_far_left[i][1] < 320 &&pts_far_left[i + 1][0] >= 0 && pts_far_left[i + 1][1] >= 0 && pts_far_left[i + 1][0] < 240 && pts_far_left[i + 1][1] < 320 )
    //     {
    //         Point startd((int)pts_far_left[i][1], (int)pts_far_left[i][0]), endd((int)pts_far_left[i + 1][1], (int)pts_far_left[i + 1][0]);
    //         line(temp1, startd, endd, Scalar(0, 0, 255), 2, LINE_AA);
    //     }
    // }
    // imwrite("line_far.jpg", temp1);
    
    //透视变换
    for (int i = 0; i < pts_far_left_count; i++) {
        pts_far_inv_l[i][1] = fclip(InverseMapW[pts_far_left[i][0]][pts_far_left[i][1]],0,IMAGE_W);
        pts_far_inv_l[i][0] = fclip(InverseMapH[pts_far_left[i][0]][pts_far_left[i][1]],0,IMAGE_H);
    }
    pts_far_inv_l_count = pts_far_left_count;

    for (int i = 0; i < pts_far_right_count; i++) {
        pts_far_inv_r[i][1] = fclip(InverseMapW[pts_far_right[i][0]][pts_far_right[i][1]],0,IMAGE_W);
        pts_far_inv_r[i][0] = fclip(InverseMapH[pts_far_right[i][0]][pts_far_right[i][1]],0,IMAGE_H);
    }
    pts_far_inv_r_count = pts_far_right_count;

    // 边线滤波
    GetLinesFilter(pts_far_inv_l, pts_far_inv_l_count, pts_far_filter_l, (int)round(FILTER_KERNELSIZE));
    pts_far_filter_l_count = pts_far_inv_l_count;
    GetLinesFilter(pts_far_inv_r, pts_far_inv_r_count, pts_far_filter_r, (int)round(FILTER_KERNELSIZE));
    pts_far_filter_r_count = pts_far_inv_r_count;

    // 边线等距采样
    pts_far_resample_left_count = PT_MAXLEN;
    GetLinesResample(pts_far_filter_l, pts_far_filter_l_count, pts_far_resample_left, &pts_far_resample_left_count, RESAMPLEDIST * PIXPERMETER);
    pts_far_resample_right_count = PT_MAXLEN;
    GetLinesResample(pts_far_filter_r, pts_far_filter_r_count, pts_far_resample_right, &pts_far_resample_right_count, RESAMPLEDIST * PIXPERMETER);

    int valid_far_points_count = 0;

    for (int i = 0; i < pts_far_resample_left_count; i++) {
        int x = pts_far_resample_left[i][1];
        int y = pts_far_resample_left[i][0];

        if (y < IMAGE_H - 3 && x > 3 && x < IMAGE_W - 3 && y >= 3) {
            pts_far_resample_left[valid_far_points_count][0] = pts_far_resample_left[i][0];
            pts_far_resample_left[valid_far_points_count][1] = pts_far_resample_left[i][1];
            valid_far_points_count++;
        }
    }
    pts_far_resample_left_count = valid_far_points_count;

    valid_far_points_count = 0;

    for (int i = 0; i < pts_far_resample_right_count; i++) {
        int x = pts_far_resample_right[i][1];
        int y = pts_far_resample_right[i][0];

        if (y <= IMAGE_H - 3 && x >= 3 && x <= IMAGE_W - 3 && y >= 3) {
            pts_far_resample_right[valid_far_points_count][0] = pts_far_resample_right[i][0];
            pts_far_resample_right[valid_far_points_count][1] = pts_far_resample_right[i][1];
            valid_far_points_count++;
        }
    }

    pts_far_resample_right_count = valid_far_points_count;

    // 边线角度变化率
    local_angle_points(pts_far_resample_left, pts_far_resample_left_count, far_angle_left, (int)round(ANGLEDIST / RESAMPLEDIST));
    far_angle_left_num = pts_far_resample_left_count;
    local_angle_points(pts_far_resample_right, pts_far_resample_right_count, far_angle_right, (int)round(ANGLEDIST / RESAMPLEDIST));
    far_angle_right_num = pts_far_resample_right_count;

    // 角度变化率非极大值抑制
    nms_angle(far_angle_left, far_angle_left_num, far_angle_new_left, (int)round(ANGLEDIST / RESAMPLEDIST) * 2 + 1);
    far_angle_new_left_num = far_angle_left_num;
    nms_angle(far_angle_right, far_angle_right_num, far_angle_new_right, (int)round(ANGLEDIST / RESAMPLEDIST) * 2 + 1);
    far_angle_new_right_num = far_angle_right_num;

    // 找远线上的L角点
    far_Lpt0_found = far_Lpt1_found = false;
    far_Lpt0_rpts0s_id = far_Lpt1_rpts1s_id = 0;
    for (int i = 0; i < pts_far_resample_left_count; i++) {
        // printf("%f ", far_angle_new_left[i] );
        if (far_angle_new_left[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_left_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_left_count - 1);
        float conf = fabs(far_angle_left[i]) - (fabs(far_angle_left[im1]) + fabs(far_angle_left[ip1])) / 2;

        // L角点阈值
        // printf("l:%f\n", conf);
        if (far_Lpt0_found == false && (40. / 180. * PI32) < conf && conf < (140. / 180. * PI32) && i < 0.5 / RESAMPLEDIST) {
            far_Lpt0_rpts0s_id = i;
            far_Lpt0_found     = true;
        }
    }

    for (int i = 0; i < pts_far_resample_right_count; i++) {
        if (far_angle_new_right[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_right_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_right_count - 1);
        float conf = fabs(far_angle_right[i]) - (fabs(far_angle_right[im1]) + fabs(far_angle_right[ip1])) / 2;

        // printf("r:%f %f\n", conf, i * RESAMPLEDIST);
        if (far_Lpt1_found == false && (40. / 180. * PI32) < conf && conf < 140. / 180. * PI32 && i < 0.5 / RESAMPLEDIST) {
            far_Lpt1_rpts1s_id = i;
            far_Lpt1_found     = true;
        }
    }
    
    if(far_Lpt0_found && far_Lpt1_found && abs(pts_far_resample_left[far_Lpt0_rpts0s_id][0] - pts_far_resample_right[far_Lpt1_rpts1s_id][0]) > 20) {
        if(pts_far_resample_left[far_Lpt0_rpts0s_id][0] < pts_far_resample_right[far_Lpt1_rpts1s_id][0])
            pts_far_resample_left_count = 0;
        else
            pts_far_resample_right_count = 0;
    }

    int farfar_Lpt0_found = false, farfar_Lpt1_found = false;
    int farfar_Lpt0_rpts0s_id = 0, farfar_Lpt1_rpts1s_id = 0;
    for (int i = far_Lpt0_rpts0s_id; i < pts_far_resample_left_count; i++) {
        // printf("%f ", far_angle_new_left[i] );
        if (far_angle_new_left[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_left_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_left_count - 1);
        float conf = fabs(far_angle_left[i]) - (fabs(far_angle_left[im1]) + fabs(far_angle_left[ip1])) / 2;

        if (far_Lpt0_found == false && (40. / 180. * PI32) < conf && conf < (140. / 180. * PI32) && i < 0.5 / RESAMPLEDIST) {
            farfar_Lpt0_rpts0s_id = i;
            farfar_Lpt0_found     = true;
        }
    }

    for (int i = far_Lpt1_rpts1s_id; i < pts_far_resample_right_count; i++) {
        if (far_angle_new_right[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_right_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_far_resample_right_count - 1);
        float conf = fabs(far_angle_right[i]) - (fabs(far_angle_right[im1]) + fabs(far_angle_right[ip1])) / 2;

        if (far_Lpt1_found == false && (40. / 180. * PI32) < conf && conf < 140. / 180. * PI32 && i < 0.5 / RESAMPLEDIST) {
            farfar_Lpt1_rpts1s_id = i;
            farfar_Lpt1_found     = true;
        }
    }
    if(farfar_Lpt0_found && !farfar_Lpt1_found)
        pts_far_resample_left_count = 0;
    else if(!farfar_Lpt0_found && farfar_Lpt1_found)
        pts_far_resample_right_count = 0;
    // printf("%d %d\n", farfar_Lpt0_rpts0s_id, farfar_Lpt1_rpts1s_id);

    // cvtColor(filtered, temp2, COLOR_GRAY2BGR);
    // // printf("%d %d\n", pts_far_resample_right_count, pts_far_resample_left_count);
    // for(int i = 0; i < pts_far_resample_left_count - 1; ++i)
    // {
    //     if(pts_far_resample_left[i][0] >= 0 && pts_far_resample_left[i][1] >= 0 && pts_far_resample_left[i][0] < 100 && pts_far_resample_left[i][1] < 200 &&pts_far_resample_left[i + 1][0] >= 0 && pts_far_resample_left[i + 1][1] >= 0 && pts_far_resample_left[i + 1][0] < 100 && pts_far_resample_left[i + 1][1] < 200 )
    //     {
    //         Point startd((int)pts_far_resample_left[i][1], (int)pts_far_resample_left[i][0]), endd((int)pts_far_resample_left[i + 1][1], (int)pts_far_resample_left[i + 1][0]);
    //         line(temp2, startd, endd, Scalar(255, 0, 0), 2, LINE_AA);
    //     }
    // }
    // for(int i = 0; i < pts_far_resample_right_count - 1; ++i)
    // {
    //     if(pts_far_resample_right[i][0] >= 0 && pts_far_resample_right[i][1] >= 0 && pts_far_resample_right[i][0] < 100 && pts_far_resample_right[i][1] < 200 &&pts_far_resample_right[i + 1][0] >= 0 && pts_far_resample_right[i + 1][1] >= 0 && pts_far_resample_right[i + 1][0] < 100 && pts_far_resample_right[i + 1][1] < 200 )
    //     {
    //         Point startd((int)pts_far_resample_right[i][1], (int)pts_far_resample_right[i][0]), endd((int)pts_far_resample_right[i + 1][1], (int)pts_far_resample_right[i + 1][0]);
    //         line(temp2, startd, endd, Scalar(0, 0, 255), 2, LINE_AA);
    //     }
    // }
    // Point center0((int)pts_far_resample_left[far_Lpt0_rpts0s_id][1], (int)pts_far_resample_left[far_Lpt0_rpts0s_id][0]);
    // Point center1((int)pts_far_resample_right[far_Lpt1_rpts1s_id][1], (int)pts_far_resample_right[far_Lpt1_rpts1s_id][0]);
    // Scalar color(225, 225, 225);
    // circle(temp2, center0, 5, color, 2);
    // circle(temp2, center1, 5, color, 2);
    // imwrite("gray.jpg", temp2);
}