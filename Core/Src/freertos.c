/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    CAN_HandleTypeDef *hcan;
    uint32_t id;
    uint8_t len;
    uint8_t *buf_data;
}can_device_transmit_member;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for CAN1_Sending_Service */
osThreadId_t CAN1_Sending_ServiceHandle;
const osThreadAttr_t CAN1_Sending_Service_attributes = {
  .name = "CAN1_Sending_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for CAN2_Sending_Service */
osThreadId_t CAN2_Sending_ServiceHandle;
const osThreadAttr_t CAN2_Sending_Service_attributes = {
  .name = "CAN2_Sending_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for DJI_Motor_Service */
osThreadId_t DJI_Motor_ServiceHandle;
const osThreadAttr_t DJI_Motor_Service_attributes = {
  .name = "DJI_Motor_Service",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for aterFull_Light_Task */
osThreadId_t aterFull_Light_TaskHandle;
const osThreadAttr_t aterFull_Light_Task_attributes = {
  .name = "aterFull_Light_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Motor_Control_Task */
osThreadId_t Motor_Control_TaskHandle;
const osThreadAttr_t Motor_Control_Task_attributes = {
  .name = "Motor_Control_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for INS_Service */
osThreadId_t INS_ServiceHandle;
const osThreadAttr_t INS_Service_attributes = {
  .name = "INS_Service",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Power_Meter */
osThreadId_t Power_MeterHandle;
const osThreadAttr_t Power_Meter_attributes = {
  .name = "Power_Meter",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Dbus_Servicce */
osThreadId_t Dbus_ServicceHandle;
const osThreadAttr_t Dbus_Servicce_attributes = {
  .name = "Dbus_Servicce",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for can1_send_fifo */
osMessageQueueId_t can1_send_fifoHandle;
const osMessageQueueAttr_t can1_send_fifo_attributes = {
  .name = "can1_send_fifo"
};
/* Definitions for can2_send_fifo */
osMessageQueueId_t can2_send_fifoHandle;
const osMessageQueueAttr_t can2_send_fifo_attributes = {
  .name = "can2_send_fifo"
};
/* Definitions for CAN1CountingSem */
osSemaphoreId_t CAN1CountingSemHandle;
const osSemaphoreAttr_t CAN1CountingSem_attributes = {
  .name = "CAN1CountingSem"
};
/* Definitions for CAN2CountingSem */
osSemaphoreId_t CAN2CountingSemHandle;
const osSemaphoreAttr_t CAN2CountingSem_attributes = {
  .name = "CAN2CountingSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void can1_sending_service(void *argument);
void can2_sending_service(void *argument);
void DJI_motor_service(void *argument);
void waterfull_light(void *argument);
void motor_control(void *argument);
void ins_service(void *argument);
void power_meter(void *argument);
void dbus_servicce(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of CAN1CountingSem */
  CAN1CountingSemHandle = osSemaphoreNew(3, 3, &CAN1CountingSem_attributes);

  /* creation of CAN2CountingSem */
  CAN2CountingSemHandle = osSemaphoreNew(3, 3, &CAN2CountingSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of can1_send_fifo */
  can1_send_fifoHandle = osMessageQueueNew (16, sizeof(can_device_transmit_member), &can1_send_fifo_attributes);

  /* creation of can2_send_fifo */
  can2_send_fifoHandle = osMessageQueueNew (16, sizeof(can_device_transmit_member), &can2_send_fifo_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of CAN1_Sending_Service */
  CAN1_Sending_ServiceHandle = osThreadNew(can1_sending_service, NULL, &CAN1_Sending_Service_attributes);

  /* creation of CAN2_Sending_Service */
  CAN2_Sending_ServiceHandle = osThreadNew(can2_sending_service, NULL, &CAN2_Sending_Service_attributes);

  /* creation of DJI_Motor_Service */
  DJI_Motor_ServiceHandle = osThreadNew(DJI_motor_service, NULL, &DJI_Motor_Service_attributes);

  /* creation of aterFull_Light_Task */
  aterFull_Light_TaskHandle = osThreadNew(waterfull_light, NULL, &aterFull_Light_Task_attributes);

  /* creation of Motor_Control_Task */
  Motor_Control_TaskHandle = osThreadNew(motor_control, NULL, &Motor_Control_Task_attributes);

  /* creation of INS_Service */
  INS_ServiceHandle = osThreadNew(ins_service, NULL, &INS_Service_attributes);

  /* creation of Power_Meter */
  Power_MeterHandle = osThreadNew(power_meter, NULL, &Power_Meter_attributes);

  /* creation of Dbus_Servicce */
  Dbus_ServicceHandle = osThreadNew(dbus_servicce, NULL, &Dbus_Servicce_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_can1_sending_service */
/**
  * @brief  Function implementing the CAN1_Sending_Service thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_can1_sending_service */
__weak void can1_sending_service(void *argument)
{
  /* USER CODE BEGIN can1_sending_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END can1_sending_service */
}

/* USER CODE BEGIN Header_can2_sending_service */
/**
* @brief Function implementing the CAN2_Sending_Service thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can2_sending_service */
__weak void can2_sending_service(void *argument)
{
  /* USER CODE BEGIN can2_sending_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END can2_sending_service */
}

/* USER CODE BEGIN Header_DJI_motor_service */
/**
* @brief Function implementing the DJI_Motor_Service thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DJI_motor_service */
__weak void DJI_motor_service(void *argument)
{
  /* USER CODE BEGIN DJI_motor_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DJI_motor_service */
}

/* USER CODE BEGIN Header_waterfull_light */
/**
* @brief Function implementing the aterFull_Light_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_waterfull_light */
__weak void waterfull_light(void *argument)
{
  /* USER CODE BEGIN waterfull_light */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END waterfull_light */
}

/* USER CODE BEGIN Header_motor_control */
/**
* @brief Function implementing the Motor_Control_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motor_control */
__weak void motor_control(void *argument)
{
  /* USER CODE BEGIN motor_control */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END motor_control */
}

/* USER CODE BEGIN Header_ins_service */
/**
* @brief Function implementing the INS_Service thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ins_service */
__weak void ins_service(void *argument)
{
  /* USER CODE BEGIN ins_service */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ins_service */
}

/* USER CODE BEGIN Header_power_meter */
/**
* @brief Function implementing the Power_Meter thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_power_meter */
__weak void power_meter(void *argument)
{
  /* USER CODE BEGIN power_meter */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END power_meter */
}

/* USER CODE BEGIN Header_dbus_servicce */
/**
* @brief Function implementing the Dbus_Servicce thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_dbus_servicce */
__weak void dbus_servicce(void *argument)
{
  /* USER CODE BEGIN dbus_servicce */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END dbus_servicce */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

