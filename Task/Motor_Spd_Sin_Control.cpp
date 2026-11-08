#include "cmsis_os2.h"
#include "DJI_motor_driver.h"
#include <math.h>

void motor_control(void *argument)
{
    DJI_motor* motor_3508 = new DJI_motor(&hcan1, nullptr, 1, DJI_M3508, 0);
    
    float full_position     = 2.0f;
    float omega             = (2.0f * 3.1415926f / 2.0f);
    float t                 = 0.0f;
    float target_position   = 0.0f;
    uint32_t start_tick = osKernelGetTickCount();
    for(;;)
    {
        target_position = full_position * ( cosf(omega * t) - 1 );  //位置(cos(wt) - 1 )对应速度sin（wt）
        motor_3508->motor_set_rounds(target_position);
        uint32_t end_tick = osKernelGetTickCount();
        t = (end_tick - start_tick) * 0.001f;
        osDelay(1);
    }
}