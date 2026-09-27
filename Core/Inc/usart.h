/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;                                     //USART1接收DMA句柄

#define USART1_RX_BUFFER_SIZE  8U                                            //USART1接收缓冲区大小

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void USART1_InitReceive(void);                                               //初始化并开启USART1接收中断
void USART1_ReceiveCallback(uint8_t data);                                   //串口接收中断回调，逐字节处理
uint8_t USART1_CheckCommand(const char *command, uint16_t length);           //判断最近是否收到指定命令
uint16_t USART1_GetRxLength(void);                                           //获取最近一帧接收长度
const uint8_t *USART1_GetRxBuffer(void);                                     //获取接收缓冲区首地址
void USART1_ClearCommand(void);                                              //清除已接收命令

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

