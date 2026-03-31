#include "math.h"
#include "gl_headfile.h"

float calculate_vector_angle(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float vector_length = sqrt(dx * dx + dy * dy);
    float angle_radians = acos(dx / vector_length);
    float angle_degrees = angle_radians * 180 / M_PI;

    return angle_degrees;
}

void get_corners()
{
    Lpt0_found = Lpt1_found = false;
    Lpt_in0_found = Lpt_in1_found = false;
    is_straight0                  = pts_resample_left_count > 0.8 / RESAMPLEDIST;
    is_straight1                  = pts_resample_right_count > 0.8 / RESAMPLEDIST;
    // is_turn0_l = is_turn1_l = is_turn0_r = is_turn1_r = 0;
    for (int i = 0; i < pts_resample_left_count; i++) {
        if (angle_new_left[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_resample_left_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_resample_left_count - 1);
        float conf = fabs(angle_left[i]) - (fabs(angle_left[im1]) + fabs(angle_left[ip1])) / 2;
         //  114_show_float(120, 80, conf, 3, 4);
            // L 角点阈值
            if (Lpt0_found == false && (66. / 180. * PI32) < conf && conf < (140. / 180. * PI32) && i < 0.5 / RESAMPLEDIST)
        {
            Lpt0_rpts0s_id = i;
            Lpt0_found     = true;
            // transform(pts_resample_left[Lpt0_rpts0s_id][1],pts_resample_left[Lpt0_rpts0s_id][0],&Lpt0[1],&Lpt0[0]);    //待优化，是否用到，无用则删
        }
        // 长直道阈值
        if (conf > (6. / 180. * PI32) && i < 0.8 / RESAMPLEDIST) {
            is_straight0 = false;
        }
        if (Lpt0_found == true && is_straight0 == false) break;
    }

    // if (Lpt0_found == false)
    // {
    //     // is_turn0 = is_curve(angle_left ,clip(angle_left_num - 10, 0,angle_left_num),0.05);
    //     int  gap = 0;
    //     gap = pts_resample_left[clip(pts_resample_left_count - 30, 0, pts_resample_left_count - 1)][1] - pts_resample_left[0][1];
    //     ips200_show_uint(200, 205, gap, 3);
    //     if (gap <= -10)
    //     {
    //         is_turn0_l = true;
    //     }
    //     if (gap >= 10)
    //     {
    //         is_turn1_l = true;
    //     }
        
    // }
    //if (is_turn0)
    //{
    // state_type = TURN_LEFT_STATE;
    //}

    Lpt0_found_barrier = Lpt1_found_barrier = false;
    Lpt0_found_barrier_in = Lpt1_found_barrier_in = false;
    for (int i = 0; i < pts_resample_left_count; i++) {
        if (angle_new_left_barrier[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST_barrier / RESAMPLEDIST), 0, pts_resample_left_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST_barrier / RESAMPLEDIST), 0, pts_resample_left_count - 1);
        float conf = fabs(angle_left_barrier[i]) - (fabs(angle_left_barrier[im1]) + fabs(angle_left_barrier[ip1])) / 2;

        // L 角点阈值
        if (Lpt0_found_barrier == false && (40. / 180. * PI32) < conf && conf < (160. / 180. * PI32) && i < 0.8 / RESAMPLEDIST) {
            Lpt0_rpts0s_id_barrier = i;
            if (pts_resample_left[i][1] <= pts_resample_left[clip(i + 2, 0, pts_resample_left_count - 1)][1])
            {
                Lpt0_found_barrier     = true;
            }
            else{
                Lpt0_found_barrier     = false;
            }
            
            //Lpt0_found_barrier     = true;
        }
    }
    if (Lpt0_found_barrier) {
        float angle1 = calculate_vector_angle(pts_resample_left[Lpt0_rpts0s_id_barrier][1], pts_resample_left[Lpt0_rpts0s_id_barrier][0], pts_resample_left[Lpt0_rpts0s_id_barrier + 5][1], pts_resample_left[Lpt0_rpts0s_id_barrier + 5][0]);
        if (angle1 < 85.) {
            Lpt0_found_barrier_in    = true;
            Lpt0_found_barrier       = false;
            Lpt0_found_barrier_in_id = Lpt0_rpts0s_id_barrier;
        }
    }




    for (int i = 0; i < pts_resample_right_count; i++) {
        if (angle_new_right[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_resample_right_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST / RESAMPLEDIST), 0, pts_resample_right_count - 1);
        float conf = fabs(angle_right[i]) - (fabs(angle_right[im1]) + fabs(angle_right[ip1])) / 2;
        // ips114_show_float(100, 100, conf, 3, 4);

        if (Lpt1_found == false && (66. / 180. * PI32) < conf && conf < 140. / 180. * PI32 && i < 0.5 / RESAMPLEDIST) {
            Lpt1_rpts1s_id = i;
            Lpt1_found     = true;
            // transform(pts_resample_right[Lpt1_rpts1s_id][1],pts_resample_right[Lpt1_rpts1s_id][0],&Lpt1[1],&Lpt1[0]);
        }

        if (conf > (6. / 180. * PI32) && i < 0.8 / RESAMPLEDIST) is_straight1 = false;
        if (Lpt1_found == true && is_straight1 == false) break;
    }

    // if(is_straight1 && is_straight0){
    //     state_type = STRAIGHT_STATE;
    // }

    // if (Lpt1_found == false)
    // {
    //     // is_turn1 = is_curve(angle_right ,clip(angle_right_num - 10, 0,angle_right_num),0.05);
    //     int  gap = 0;
    //     gap = pts_resample_right[clip(pts_resample_right_count - 30, 0, pts_resample_right_count - 1)][1] - pts_resample_right[0][1];
    //     ips200_show_uint(200, 224, gap, 3);
    //     if (gap <= -10)
    //     {
    //         is_turn1_l = true;
    //     }
    //     if (gap >= 10)
    //     {
    //         is_turn1_r = true;
    //     }
    // }

    for (int i = 0; i < pts_resample_right_count; i++) {
        if (angle_new_right_barrier[i] == 0) continue;
        int im1    = clip(i - (int)round(ANGLEDIST_barrier / RESAMPLEDIST), 0, pts_resample_right_count - 1);
        int ip1    = clip(i + (int)round(ANGLEDIST_barrier / RESAMPLEDIST), 0, pts_resample_right_count - 1);
        float conf = fabs(angle_right_barrier[i]) - (fabs(angle_right_barrier[im1]) + fabs(angle_right_barrier[ip1])) / 2;

        if (Lpt1_found_barrier == false && (40. / 180. * PI32) < conf && conf < 160. / 180. * PI32 && i < 0.8 / RESAMPLEDIST) {
            Lpt1_rpts1s_id_barrier = i;

            if (pts_resample_right[i][1] >= pts_resample_right[clip(i + 2, 0, pts_resample_left_count - 1)][1])
            {
                Lpt1_found_barrier     = true;
            }
            else{
                Lpt1_found_barrier     = false;
            }
            //Lpt1_found_barrier     = true;
        }
    }
    if (Lpt1_found_barrier) {
        float angle2 = calculate_vector_angle(pts_resample_right[Lpt1_rpts1s_id_barrier][1], pts_resample_right[Lpt1_rpts1s_id_barrier][0], pts_resample_right[Lpt1_rpts1s_id_barrier + 5][1], pts_resample_right[Lpt1_rpts1s_id_barrier + 5][0]);
        if (angle2 > 100.) {
            Lpt1_found_barrier_in    = true;
            Lpt1_found_barrier       = false;
            Lpt1_found_barrier_in_id = Lpt1_rpts1s_id_barrier;
        }
    }

    // if (is_turn0_l && is_turn1_l)
    // {
    //     state_type = TURN_LEFT_STATE; 
    // }
    // if (is_turn0_r && is_turn1_r)
    // {
    //     state_type = TURN_RIGHT_STATE;
    // }
}
