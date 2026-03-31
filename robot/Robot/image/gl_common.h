#ifndef COMMON_H_
#define COMMON_H_


#define MT9V03X_H             (240)
#define MT9V03X_W             (320)
#define IMAGE_H               (MT9V03X_H)
#define IMAGE_W               (MT9V03X_W)
#define BEGINH_L              (140)
#define BEGINH_L_curvature    (230)
#define BEGINH_R              (140)
#define BEGINH_R_curvature    (230)
#define BEGINW_L              (-20)
#define BEGINW_R              (-20)
#define PT_MAXLEN             (150)
#define GET_PIX_1C(IMG, H, W) (IMG[(H) * MT9V03X_W + (W)]) // 获取像素点的值
#define SELFADAPT_KERNELSIZE  (3)                          // 巡线区域核大小
#define FILTER_KERNELSIZE     (3)                          // 滤波核大小
#define SELFADAPT_OFFSET      (4)                          // 适应性块大小
#define PIXPERMETER           (100.0f)
#define PIXPERMETER_ACROSS    (100.0f)
#define PIXPERMETER_ACROSS_BARRIER    (10.0f)
#define RESAMPLEDIST          (0.02f)
#define ANGLEDIST             (0.2f)
#define ANGLEDIST_barrier     (0.06f)
#define ROADWIDTH             (0.55f)
#define FRAMENONE             (1)
#define FRAMETOLEFT           (5)
#define FRAMETORIGHT          (5)

#define PI32                  (3.1415926535898f)
#include <gl_headfile.h>

int32_t limit(int32_t x, int32_t low, int32_t up);
int clip(int x, int low, int up);
float fclip(float x, float low, float up);

#endif