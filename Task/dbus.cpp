#include "cmsis_os2.h"
#include "dbus.h"
#include "usart.h"

static uint8_t s_dma_buf[18];
static RcData_t s_rc;
static volatile uint8_t s_frame_armed;

void Dbus_Init()
{
    if (HAL_UARTEx_ReceiveToIdle_DMA(
            &huart3, s_dma_buf, DBUS_FRAME_LEN) != HAL_OK)
    {
        Error_Handler();
    }
}

float rc_normalize(uint16_t raw)
{
    float value =
        ((float)raw - (float)(DBUS_CH_MIN + DBUS_CH_MAX) / 2) /
        ((float)(DBUS_CH_MAX - DBUS_CH_MIN) / 2);

    return value;
}

uint8_t Rc_Unpack(const uint8_t *raw, RcData_t *out)
{
    uint16_t ch[4];

    ch[0] = (uint16_t)((raw[0] | (raw[1] << 8)) & 0x07FFu);
    ch[1] = (uint16_t)(((raw[1] >> 3) | (raw[2] << 5)) & 0x07FFu);
    ch[2] = (uint16_t)(((raw[2] >> 6) |
                        (raw[3] << 2) |
                        (raw[4] << 10)) & 0x07FFu);
    ch[3] = (uint16_t)(((raw[4] >> 1) | (raw[5] << 7)) & 0x07FFu);

    for (int i = 0; i < 4; i++)
    {
        if (ch[i] > DBUS_CH_MAX || ch[i] < DBUS_CH_MIN)
        {
            return 0;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        out->ch[i] = rc_normalize(ch[i]);
    }

    uint8_t s[2];

    s[0] = (uint8_t)((raw[5] >> 6) & 0x03u);
    s[1] = (uint8_t)((raw[5] >> 4) & 0x03u);

    for (int i = 0; i < 2; i++)
    {
        if (s[i] != 1 && s[i] != 2 && s[i] != 3)
        {
            return 0;
        }
    }

    out->s1 = s[0];
    out->s2 = s[1];

    return 1;
}

void Dbus_Read(RcData_t *out)
{
    *out = s_rc;
}

void Dbus_Is_Valid()
{
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();

    s_rc.is_valid =
        ((HAL_GetTick() - s_rc.rx_ms) <= RC_LINK_TIMEOUT_MS) ? 1u : 0u;

    __set_PRIMASK(primask);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    RcData_t rc;

    if (huart->Instance != huart3.Instance)
    {
        return;
    }

    if (huart->RxEventType == HAL_UART_RXEVENT_TC)
    {
        if ((s_frame_armed == 0u) || (Size != DBUS_FRAME_LEN))
        {
            return;
        }

        if (Rc_Unpack(s_dma_buf, &rc) != 0u)
        {
            rc.rx_ms = HAL_GetTick();
            rc.is_valid =
                ((rc.rx_ms - s_rc.rx_ms) <= RC_LINK_TIMEOUT_MS) ? 1u : 0u;

            s_rc = rc;
        }

        return;
    }

    if (huart->RxEventType == HAL_UART_RXEVENT_IDLE)
    {
        if (Size == DBUS_FRAME_LEN)
        {
            s_frame_armed = 1u;
            return;
        }

        s_frame_armed = 0u;

        (void)HAL_UART_AbortReceive(&huart3);
        (void)HAL_UARTEx_ReceiveToIdle_DMA(
            &huart3, s_dma_buf, DBUS_FRAME_LEN);

        return;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != huart3.Instance)
    {
        return;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    s_frame_armed = 0u;

    (void)HAL_UART_AbortReceive(&huart3);
    (void)HAL_UARTEx_ReceiveToIdle_DMA(
        &huart3, s_dma_buf, DBUS_FRAME_LEN);
}

void dbus_servicce(void *argument)
{
    Dbus_Init();

    /* USER CODE BEGIN dbus_servicce */
    /* Infinite loop */
    for (;;)
    {
        Dbus_Is_Valid();
        osDelay(1);
    }
    /* USER CODE END dbus_servicce */
}