#ifndef TIMER_H
#define TIMER_H

#include <cstdio>
#include "loongson.h"
#include "zf_common_headfile.h"


#include "machine.h"


extern int timeStamp;

void pit1_callback();

#endif