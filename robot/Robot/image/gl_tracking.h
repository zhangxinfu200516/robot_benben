#ifndef TRACKING
#define TRACKING
#include <gl_headfile.h>

extern float (*mid_track)[2];
extern int32_t mid_track_count;
extern float pure_angle;
extern float pure_angle_half;
extern float dx_near;

extern int8_t turn_flag ;
extern float curvature;
extern float turn_threshold;
void aim_distance_select(void);
void tracking(void);
void ElementJudge(void);
void ElementRun(void);
void MidLineTrack(void); 


#endif
