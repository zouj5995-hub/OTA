#include "flash_ops.h"

FLASH_OpsResult_t FlashOps_ReadWord(uint32_t address, uint32_t *value)
{
  if ((value == 0) || ((address & 0x3U) != 0U))
  {
    return FLASH_OPS_ERROR_PROGRAM;
  }

  *value = *(volatile uint32_t *)address;
  return FLASH_OPS_OK;
}

FLASH_OpsResult_t FlashOps_ErasePage(uint32_t address, uint32_t page_count)
{
  FLASH_EraseInitTypeDef erase_cfg = {0};
  uint32_t page_error = 0U;

  if ((page_count == 0U) || ((address & (FLASH_PAGE_SIZE - 1U)) != 0U))
  {
    return FLASH_OPS_ERROR_ERASE;
  }

  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  erase_cfg.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_cfg.PageAddress = address;
  erase_cfg.NbPages = page_count;

  if (HAL_FLASHEx_Erase(&erase_cfg, &page_error) != HAL_OK)
  {
    (void)HAL_FLASH_Lock();
    return FLASH_OPS_ERROR_ERASE;
  }

  if (HAL_FLASH_Lock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  return FLASH_OPS_OK;
}

FLASH_OpsResult_t FlashOps_WriteWord(uint32_t address, uint32_t value)
{
  if ((address & 0x3U) != 0U)
  {
    return FLASH_OPS_ERROR_PROGRAM;
  }

  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value) != HAL_OK)
  {
    (void)HAL_FLASH_Lock();
    return FLASH_OPS_ERROR_PROGRAM;
  }

  if (HAL_FLASH_Lock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  return FLASH_OPS_OK;
}

FLASH_OpsResult_t FlashOps_Write(uint32_t address, const uint32_t *data, uint16_t word_count)
{
  uint16_t index;

  if ((data == 0) || (word_count == 0U) || ((address & 0x3U) != 0U))
  {
    return FLASH_OPS_ERROR_PROGRAM;
  }

  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  for (index = 0U; index < word_count; index++)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          address + (index * 4U),
                          data[index]) != HAL_OK)
    {
      (void)HAL_FLASH_Lock();
      return FLASH_OPS_ERROR_PROGRAM;
    }
  }

  if (HAL_FLASH_Lock() != HAL_OK)
  {
    return FLASH_OPS_ERROR_UNLOCK;
  }

  return FLASH_OPS_OK;
}

FLASH_OpsResult_t FlashOps_ClearPage(uint32_t address, uint32_t page_count)
{
  return FlashOps_ErasePage(address, page_count);
}

uint8_t FlashOps_IsUpgradeFlagSet(uint32_t address, uint32_t magic)
{
  uint32_t value;

  if (FlashOps_ReadWord(address, &value) != FLASH_OPS_OK)
  {
    return 0U;
  }

  return (value == magic) ? 1U : 0U;
}

FLASH_OpsResult_t FlashOps_SetUpgradeFlag(uint32_t address, uint32_t magic)
{
  if (FlashOps_ErasePage(address, 1U) != FLASH_OPS_OK)
  {
    return FLASH_OPS_ERROR_ERASE;
  }

  return FlashOps_WriteWord(address, magic);
}

FLASH_OpsResult_t FlashOps_ClrUpgradeFlag(uint32_t address)
{
  return FlashOps_ErasePage(address, 1U);
}
