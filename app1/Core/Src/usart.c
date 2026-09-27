/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
static uint8_t usart1_rx_buffer[USART1_RX_BUFFER_SIZE];  //USART1接收缓冲区
static volatile uint16_t usart1_rx_length = 0U;          //最近一帧接收长度
static volatile uint8_t usart1_rx_complete = 0U;         //命令接收完成标志
/* USER CODE END PV */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * 函数名：USART1_InitReceive
  * 功能：初始化接收状态并开启USART1接收中断
  * 参数：无
  * 返回值：无
  * 说明：上电后调用一次，之后由接收中断逐字节处理
  */
void USART1_InitReceive(void)
{
  /*==============================
   *  #1. 复位接收状态
   *==============================*/
  usart1_rx_length = 0U;                                        //接收长度清零
  usart1_rx_complete = 0U;                                      //命令完成标志清零

  /*==============================
   *  #2. 开启接收中断
   *==============================*/
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);                  //开启接收非空中断
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);                   //开启错误中断
}

/**
  * 函数名：USART1_ReceiveCallback
  * 功能：串口接收中断回调，逐字节处理接收数据
  * 参数：data：本次接收到的字节
  * 返回值：无
  * 说明：由USART1中断服务函数调用，内部不做阻塞操作
  */
void USART1_ReceiveCallback(uint8_t data)
{
  /*==============================
   *  #1. 丢弃帧间空白字符
   *==============================*/
  if ((data == '\r') || (data == '\n'))                          //丢弃回车换行
  {
    return;
  }

  if (data == ' ')                                               //丢弃帧间空格
  {
    return;
  }

  /*==============================
   *  #2. 保存接收字节
   *==============================*/
  if (usart1_rx_length < USART1_RX_BUFFER_SIZE)                  //缓冲区未满
  {
    usart1_rx_buffer[usart1_rx_length] = data;                   //保存接收字节
    usart1_rx_length++;                                          //接收长度加一
  }
  else                                                           //缓冲区已满
  {
    uint16_t i;

    for (i = 1U; i < USART1_RX_BUFFER_SIZE; i++)                 //向左平移一个字节
    {
      usart1_rx_buffer[i - 1U] = usart1_rx_buffer[i];
    }

    usart1_rx_buffer[USART1_RX_BUFFER_SIZE - 1U] = data;         //新字节放到末尾
  }

  usart1_rx_complete = 1U;                                       //置位命令完成标志
}

/**
  * 函数名：USART1_CheckCommand
  * 功能：判断最近一帧内容是否为指定命令
  * 参数：command：命令字符串；length：命令长度
  * 返回值：1：匹配；0：不匹配
  * 说明：命令比较不区分大小写
  */
uint8_t USART1_CheckCommand(const char *command, uint16_t length)
{
  uint16_t i;
  uint16_t rx_length;

  /*==============================
   *  #1. 参数和长度检查
   *==============================*/
  if ((command == 0) || (length == 0U))                          //参数无效
  {
    return 0U;
  }

  if (usart1_rx_complete == 0U)                                  //尚未收到任何数据
  {
    return 0U;
  }

  rx_length = usart1_rx_length;                                  //取出本帧长度
  if (rx_length != length)                                       //长度不一致
  {
    return 0U;
  }

  /*==============================
   *  #2. 逐字节比较命令
   *==============================*/
  for (i = 0U; i < length; i++)
  {
    char received;
    char expected;

    received = (char)usart1_rx_buffer[i];                        //取出接收字符
    expected = command[i];                                       //取出命令字符

    if ((received >= 'A') && (received <= 'Z'))                  //接收字符转小写
    {
      received = (char)(received + ('a' - 'A'));
    }

    if ((expected >= 'A') && (expected <= 'Z'))                  //命令字符转小写
    {
      expected = (char)(expected + ('a' - 'A'));
    }

    if (received != expected)                                    //字符不一致
    {
      return 0U;
    }
  }

  return 1U;                                                     //命令匹配
}

/**
  * 函数名：USART1_ClearCommand
  * 功能：清除已接收命令，便于接收下一条命令
  * 参数：无
  * 返回值：无
  * 说明：命令处理完成后调用
  */
void USART1_ClearCommand(void)
{
  usart1_rx_length = 0U;                                         //接收长度清零
  usart1_rx_complete = 0U;                                       //命令完成标志清零
}

/**
  * 函数名：USART1_GetRxLength
  * 功能：获取最近一帧接收长度
  * 参数：无
  * 返回值：最近一帧接收到的字节数
  * 说明：配合USART1_CheckCommand使用
  */
uint16_t USART1_GetRxLength(void)
{
  return usart1_rx_length;                                        //返回接收长度
}

/**
  * 函数名：USART1_GetRxBuffer
  * 功能：获取接收缓冲区首地址
  * 参数：无
  * 返回值：接收缓冲区首地址
  * 说明：用于调试时回显收到的原始数据
  */
const uint8_t *USART1_GetRxBuffer(void)
{
  return usart1_rx_buffer;                                        //返回接收缓冲区首地址
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
