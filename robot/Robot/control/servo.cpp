#include "servo.h"


// float servo_set_mid=95;
float servo_set_mid=90;
float servo_set_max=110;        //左
float servo_set_min=78;         //右

PwmController SERVO(SERVO_pin,0,1);


void servo_set(float diff)
{
    if(diff>40)diff=40;
    else if(diff<-40)diff=-40;

    float servo_motor_duty=diff*0.4+servo_set_mid;
    SERVO.setDutyCycle((unsigned int)SERVO_MOTOR_DUTY(servo_motor_duty));
}

void servo_init(void)
{
    SERVO.initialize();
    SERVO.setPeriod(SERVO_MOTOR_PERIOD);       //10^6
    servo_set(0);
    SERVO.enable();
}
