#include "gl_headfile.h"

int32_t limit(int32_t x, int32_t low, int32_t up)
{
    return x > up ? up : x < low ? low
                                 : x;
}
int clip(int x, int low, int up)
{
    return x > up ? up : x < low ? low
                                 : x;
}

float fclip(float x, float low, float up)
{
    return x > up ? up : x < low ? low
                                 : x;
}