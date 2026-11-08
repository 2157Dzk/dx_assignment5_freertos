#include "Power_Meter.h"
#include "CAN_bsp.h"
#include <string.h>

static volatile uint32_t s_power_meter_raw;
static volatile PowerMeterData_t s_power_meter;

static void power_meter_rx_callback(can_device_receive *, uint8_t *data)
{
    uint32_t raw;

    if (data == nullptr)
    {
        return;
    }

    memcpy(&raw, data, sizeof(raw));
    s_power_meter_raw = raw;
    s_power_meter.ms = HAL_GetTick();
    s_power_meter.is_valid = 1u;
}

static void power_meter_unpack()
{
    uint32_t raw = s_power_meter_raw;
    uint32_t primask;
    uint16_t power_raw = (uint16_t)(raw & 0xFFFFu);
    uint16_t energy_raw = (uint16_t)(raw >> 16);

    primask = __get_PRIMASK();
    __disable_irq();

    s_power_meter.chassis_power_w =
        (float)(int16_t)power_raw * 0.1f;

    s_power_meter.buffer_energy_j =
        (float)(int16_t)energy_raw * 0.1f;

    __set_PRIMASK(primask);
}

void PowerMeter_Is_Valid(void)
{
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();

    if ((s_power_meter.is_valid != 0u) &&
        ((HAL_GetTick() - s_power_meter.ms) >
         PowerMeter_TIMEOUT_MS))
    {
        s_power_meter.is_valid = 0u;
    }
    __set_PRIMASK(primask);
}

void PowerMeter_Read(PowerMeterData_t *out)
{
    uint32_t primask;

    if (out == nullptr)
    {
        return;
    }

    primask = __get_PRIMASK();
    __disable_irq();
    out->chassis_power_w = s_power_meter.chassis_power_w;
    out->buffer_energy_j = s_power_meter.buffer_energy_j;
    out->ms = s_power_meter.ms;
    out->is_valid = s_power_meter.is_valid;
    __set_PRIMASK(primask);
}

void power_meter(void *argument)
{
    can_device_receive power_meter_can(&hcan1, 0x31, power_meter_rx_callback);
    for (;;)
    {
        PowerMeter_Is_Valid();
        power_meter_unpack();
        osDelay(1);
    }
}