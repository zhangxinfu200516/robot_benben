#ifndef BARRIER
#define BARRIER

enum barrier_type_e {
  BARRIER_NONE,
  BARRIER_RUNNING,
};

enum s_type_e {
  S_NONE,
  S_BEGIN,
  S_RUNNING,
  S_OUT,
};

enum shield_type_e {
  SHIELD_NONE,
  SHIELD_BEGIN,
};

extern enum barrier_type_e barrier_type;
extern enum s_type_e s_type;
extern enum shield_type_e shield_type;

void CheckBarrier(void);
void RunBarrier(void);
#endif 