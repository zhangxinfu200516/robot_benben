#ifndef IMG_PROCESS
#define IMG_PROCESS
#include <gl_headfile.h>


void img_processing(void);
void SearchLineAdaptive_Left(uint8_t img_gray[], int32_t block_size, int32_t down_value, int32_t h, int32_t w, int32_t pts[][2], int32_t* line_num);
void SearchLineAdaptive_Right(uint8_t img_gray[], int32_t block_size, int32_t down_value, int32_t h, int32_t w, int32_t pts[][2], int32_t* line_num);

#endif
