#ifndef STATE_H_
#define STATE_H_

enum state_type_e {
    COMMON_STATE,
    TURN_STATE,
    STRAIGHT_STATE,
    CIRCLE_STATE,
    CIRCLE_RUNNING_STATE,
    BARRIER_STATE,
};
extern enum state_type_e last_state;
extern enum state_type_e state_type;
extern float cricle_aim;
extern float cross_aim;
extern float common_aim;
extern float straight_aim; 
extern float turn_aim;  
extern float mid_aim;
extern float barrier_aim;
extern float barrier_offset;

#endif
