#ifndef DX_ASSIGNMENT5_FREERTOS_DBUS_H
#define DX_ASSIGNMENT5_FREERTOS_DBUS_H

#include "usart.h"

#define RC_LINK_TIMEOUT_MS  21u
#define DBUS_FRAME_LEN      18u
#define DBUS_CH_MIN         364
#define DBUS_CH_MAX         1684

typedef struct
{
    float    ch[4];
    uint8_t  s1;
    uint8_t  s2;
    uint32_t rx_ms;
    uint8_t  is_valid;
} RcData_t;

void Dbus_Init();

uint8_t Rc_Unpack(const uint8_t *raw, RcData_t *out);

float rc_normalize(uint16_t raw);

void Dbus_Is_Valid();

#ifdef __cplusplus
extern "C" {
#endif

void Dbus_Read(RcData_t *out);
void dbus_servicce(void *argument);

#ifdef __cplusplus
}
#endif

#endif // DX_ASSIGNMENT5_FREERTOS_DBUS_H