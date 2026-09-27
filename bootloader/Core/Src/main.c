/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "flash_ops.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static const uint8_t bootloader_start_msg[] = "Bootloader-STAR\r\n";
static const uint8_t upgrade_mode_msg[] = "UpData Star\r\n";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void JumpToApp(uint32_t app_address);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define APP_A_ADDRESS 0x08004000U
#define APP_B_ADDRESS 0x08009800U
#define FLAG_ADDR 0x0800F000U
#define UPGRADE_FLAG 0x55AA55AAU

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_UART_Transmit(&huart1, (uint8_t *)bootloader_start_msg,
                        sizeof(bootloader_start_msg) - 1U, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }

  if (FlashOps_IsUpgradeFlagSet(FLAG_ADDR, UPGRADE_FLAG))
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)upgrade_mode_msg,sizeof(upgrade_mode_msg) - 1U, HAL_MAX_DELAY);
		int i =0;
		for(i=0;i<3;i++)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)"Updata ing...\r\n",sizeof("Updata ing...\r\n") - 1U, HAL_MAX_DELAY);
			HAL_Delay(1000);
		}
		HAL_UART_Transmit(&huart1, (uint8_t *)"Updata finish\r\n",sizeof("Updata finish\r\n") - 1U, HAL_MAX_DELAY);
			
			//#1. 清除升级标志
			if (FlashOps_ClrUpgradeFlag(FLAG_ADDR) != FLASH_OPS_OK)
			{
			  Error_Handler();
			}
			
			//#2. 复位
			HAL_NVIC_SystemReset();
			
  }
  else
  {
    JumpToApp(APP_A_ADDRESS);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
typedef void (*pFuntion)(void);
static void JumpToApp(uint32_t app_address)
{
	//#1. 声明函数变量
	uint32_t JumpAddress_REST;//存APP复位函数地址
	pFuntion JumpToApplication;//APP复位函数
	
	//#2. 提取复位函数地址，设为函数指针
	JumpAddress_REST = *(volatile uint32_t *)(app_address + 4U);
	JumpToApplication = (pFuntion)JumpAddress_REST;
	
	//#3. 关闭所有中断
	__disable_irq();
	
	//#4. 设置中断向量表和堆栈指针
	SCB->VTOR = app_address;
	__set_MSP(*(volatile uint32_t *)app_address);
	
	//#5. 执行复位
	JumpToApplication();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
