/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash.c
  * @brief   Flash helper functions.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "flash.h"

#define FLASH_UPGRADE_FLAG_ADDRESS 0x0800F000U
#define FLASH_UPGRADE_FLAG_VALUE   0x55AA55AAU
#define FLASH_PAGE_SIZE            0x400U
#define FLASH_START_ADDRESS        0x08000000U
#define FLASH_END_ADDRESS          0x08010000U

static uint8_t Flash_IsRangeValid(uint32_t address, uint32_t size)
{
  if (size == 0U)
  {
    return 0U;
  }

  if (address < FLASH_START_ADDRESS)
  {
    return 0U;
  }

  if (address > (FLASH_END_ADDRESS - size))
  {
    return 0U;
  }

  return 1U;
}

static HAL_StatusTypeDef Flash_ErasePageUnlocked(uint32_t page_address, uint32_t page_count)
{
  FLASH_EraseInitTypeDef erase_cfg = {0};
  uint32_t page_error = 0U;

  erase_cfg.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_cfg.PageAddress = page_address;
  erase_cfg.NbPages = page_count;

  return HAL_FLASHEx_Erase(&erase_cfg, &page_error);
}

HAL_StatusTypeDef Flash_ErasePage(uint32_t page_address, uint32_t page_count)
{
  HAL_StatusTypeDef status;

  if ((page_count == 0U) ||
      ((page_address % FLASH_PAGE_SIZE) != 0U))
  {
    return HAL_ERROR;
  }

  if (Flash_IsRangeValid(page_address, page_count * FLASH_PAGE_SIZE) == 0U)
  {
    return HAL_ERROR;
  }

  status = HAL_FLASH_Unlock();
  if (status != HAL_OK)
  {
    return status;
  }

  status = Flash_ErasePageUnlocked(page_address, page_count);
  (void)HAL_FLASH_Lock();

  return status;
}

HAL_StatusTypeDef Flash_WriteWord(uint32_t address, uint32_t data)
{
  HAL_StatusTypeDef status;

  if ((address % sizeof(uint32_t)) != 0U)
  {
    return HAL_ERROR;
  }

  if (Flash_IsRangeValid(address, sizeof(uint32_t)) == 0U)
  {
    return HAL_ERROR;
  }

  status = HAL_FLASH_Unlock();
  if (status != HAL_OK)
  {
    return status;
  }

  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
  (void)HAL_FLASH_Lock();

  return status;
}

HAL_StatusTypeDef Flash_WriteBuffer(uint32_t address, const uint32_t *data, uint32_t word_count)
{
  HAL_StatusTypeDef status;
  uint32_t index;

  if ((data == NULL) || (word_count == 0U))
  {
    return HAL_ERROR;
  }

  if ((address % sizeof(uint32_t)) != 0U)
  {
    return HAL_ERROR;
  }

  if (Flash_IsRangeValid(address, word_count * sizeof(uint32_t)) == 0U)
  {
    return HAL_ERROR;
  }

  status = HAL_FLASH_Unlock();
  if (status != HAL_OK)
  {
    return status;
  }

  for (index = 0U; index < word_count; index++)
  {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                               address + (index * sizeof(uint32_t)),
                               data[index]);
    if (status != HAL_OK)
    {
      break;
    }
  }

  (void)HAL_FLASH_Lock();

  return status;
}

uint32_t Flash_ReadWord(uint32_t address)
{
  return *(volatile uint32_t *)address;
}

HAL_StatusTypeDef Flash_ClearUpgradeFlag(void)
{
  return Flash_ErasePage(FLASH_UPGRADE_FLAG_ADDRESS, 1U);
}

HAL_StatusTypeDef Flash_SetUpgradeFlag(void)
{
  return Flash_WriteWord(FLASH_UPGRADE_FLAG_ADDRESS, FLASH_UPGRADE_FLAG_VALUE);
}

uint8_t Flash_IsUpgradeFlagSet(void)
{
  return (Flash_ReadWord(FLASH_UPGRADE_FLAG_ADDRESS) == FLASH_UPGRADE_FLAG_VALUE) ? 1U : 0U;
}
