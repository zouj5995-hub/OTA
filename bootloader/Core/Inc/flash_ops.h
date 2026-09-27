#ifndef __FLASH_OPS_H__
#define __FLASH_OPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  FLASH_OPS_OK = 0,
  FLASH_OPS_ERROR_UNLOCK,
  FLASH_OPS_ERROR_ERASE,
  FLASH_OPS_ERROR_PROGRAM
} FLASH_OpsResult_t;

FLASH_OpsResult_t FlashOps_ReadWord(uint32_t address, uint32_t *value);
FLASH_OpsResult_t FlashOps_ErasePage(uint32_t address, uint32_t page_count);
FLASH_OpsResult_t FlashOps_WriteWord(uint32_t address, uint32_t value);
FLASH_OpsResult_t FlashOps_Write(uint32_t address, const uint32_t *data, uint16_t word_count);
FLASH_OpsResult_t FlashOps_ClearPage(uint32_t address, uint32_t page_count);

uint8_t FlashOps_IsUpgradeFlagSet(uint32_t address, uint32_t magic);
FLASH_OpsResult_t FlashOps_SetUpgradeFlag(uint32_t address, uint32_t magic);
FLASH_OpsResult_t FlashOps_ClrUpgradeFlag(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_OPS_H__ */
