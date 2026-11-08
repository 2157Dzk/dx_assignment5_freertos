#ifndef DX_ASSIGNMENT5_FREERTOS_POWER_METER_H
#define DX_ASSIGNMENT5_FREERTOS_POWER_METER_H

#include <cstdint>
#define PowerMeter_TIMEOUT_MS  10u
typedef struct
{
    float chassis_power_w;
    float buffer_energy_j;
    uint32_t ms;
    uint8_t is_valid;
} PowerMeterData_t;

void PowerMeter_Is_Valid();
static void power_meter_unpack();

#ifdef __cplusplus
extern "C" {
#endif

void power_meter(void *argument);
void PowerMeter_Read(PowerMeterData_t *out);

#ifdef __cplusplus
}
#endif

#endif //DX_ASSIGNMENT5_FREERTOS_POWER_METER_H
