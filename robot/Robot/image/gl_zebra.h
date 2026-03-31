#ifndef ZEBRA
#define ZEBRA

#include "zf_common_headfile.h"
#include "timer.h"

enum zebra_type_e {
  ZEBRA_NONE,
  ZEBRA_START,
  ZEBRA_RUNNING,
};

extern enum zebra_type_e zebra_type;
void CheckZebra(void);
void RunZebra(void);
extern int time_begin;
#endif