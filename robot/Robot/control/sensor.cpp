#include "sensor.h"
#include "config.h"

// ==============================
// 编码器硬件配置
// ==============================
ENCODER L_encoder(R_encoder_PWMpin, R_encoder_GPIOpin);
ENCODER R_encoder(L_encoder_PWMpin, L_encoder_GPIOpin);

// ==============================
// 编码器数据更新接口
// ==============================
void sensor_encoder_get(void)
{
    // 更新左右轮速度（取绝对值）
    L_speed =  L_encoder.pulse_counter_update();
    L_speed = fabs(L_speed);
    
    R_speed =  R_encoder.pulse_counter_update();
    R_speed = fabs(R_speed);

    if(L_speed>=5) std::cout<<"Lspeed: "<<L_speed<<std::endl;
    if(R_speed>=5) std::cout<<"Rspeed: "<<R_speed<<std::endl;
}