/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash.h
  * @brief   Flash helper functions.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

uint32_t Flash_ReadWord(uint32_t address);
HAL_StatusTypeDef Flash_ErasePage(uint32_t page_address, uint32_t page_count);
HAL_StatusTypeDef Flash_WriteWord(uint32_t address, uint32_t data);
HAL_StatusTypeDef Flash_WriteBuffer(uint32_t address, const uint32_t *data, uint32_t word_count);
HAL_StatusTypeDef Flash_ClearUpgradeFlag(void);
HAL_StatusTypeDef Flash_SetUpgradeFlag(void);
uint8_t Flash_IsUpgradeFlagSet(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_H__ */
