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
#include <string.h>
#include <stdio.h>
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
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

// UART Related
void sendUART(UART_HandleTypeDef *huart, char inp[]) {
    HAL_UART_Transmit(huart, (uint8_t *)inp, strlen(inp), HAL_MAX_DELAY);
    HAL_UART_Transmit(huart, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
}

void recieveUART(UART_HandleTypeDef *huart, char* buf,int size) {
	uint8_t c;
	int lInd = 0;
	while (1) {
		if (HAL_UART_Receive(huart, &c, 1, HAL_MAX_DELAY) == HAL_OK) {
			if ((c == '\r') || (c == '\n')) {
				if (lInd > 0) {
					buf[lInd] = '\0';
					return;
				}
			} else if (lInd < size - 1) {
				buf[lInd++] = c;
			}
		}
	}
}

// Wifi Related
int sendATCommand(UART_HandleTypeDef *huart, char atCMD[],char *output,int outputSize) {
	output[0] = '\0';
	sendUART(huart, atCMD);
	char recBuf[64];
	while (1) {
		recieveUART(huart, recBuf, 64);
		strncat(output,recBuf, outputSize - strlen(output) - 1);
		strncat(output,"\r\n", outputSize - strlen(output) - 1);
		if (strcmp(recBuf, "OK") == 0) {
			return 1;
		} else if (strcmp(recBuf, "ERROR") == 0 || strcmp(recBuf, "FAIL") == 0) {
			return 0;
		}
	}
}

int initWifi(UART_HandleTypeDef *huart) {
	HAL_Delay(2000);
	__HAL_UART_CLEAR_OREFLAG(huart);
	char buf[32];
	sendATCommand(huart, "ATE0", buf, sizeof(buf));
	return sendATCommand(huart, "AT", buf, sizeof(buf));
}

int setMode(UART_HandleTypeDef *huart, char mode[], char *outBuf,int outBufSize) {
	outBuf[0] = '\0';
	char toSend[16];
	if (strcmp(mode,"STATION") == 0) {
		strcpy(toSend,"AT+CWMODE=1");
	} else if (strcmp(mode,"AP") == 0) {
		strcpy(toSend,"AT+CWMODE=2");
	} else if (strcmp(mode,"STATION_AP") == 0) {
		strcpy(toSend,"AT+CWMODE=3");
	} else {
		return -1;
	}
	return sendATCommand(huart, toSend, outBuf, outBufSize);
}

int joinAP(UART_HandleTypeDef *huart, char ssid[], char password[], char *outBuf,int outBufSize) {
	outBuf[0] = '\0';
	char toSend[128] = "AT+CWJAP=\"";
	strcat(toSend, ssid);
	strcat(toSend, "\",\"");
	strcat(toSend,password);
	strcat(toSend,"\"");
	return sendATCommand(huart, toSend, outBuf, outBufSize);
}

int getMAC(UART_HandleTypeDef *huart, char* macBuf,int macBufSize) {
	macBuf[0] = '\0';
	char outBuf[256];
	int ret = sendATCommand(huart, "AT+CIFSR", outBuf, sizeof(outBuf));
	if (ret) {
		char *mac = strstr(outBuf,"STAMAC");
		if (mac == NULL) {
			return 0;
		} else {
			mac += 8;
			int lastInd = 0;
			while (lastInd < macBufSize - 1 && *mac != '\0') {
				if (*mac == '\"') break;
				macBuf[lastInd++] = *mac;
				mac++;
			}
			macBuf[lastInd] = '\0';
		}
	}
	return ret;
}

int getIP(UART_HandleTypeDef *huart, char* ipBuf,int ipBufSize) {
	ipBuf[0] = '\0';
	char outBuf[256];
	int ret = sendATCommand(huart, "AT+CIFSR", outBuf, sizeof(outBuf));
	if (ret) {
		char *ip = strstr(outBuf,"STAIP");
		if (ip == NULL) {
			return 0;
		} else {
			ip += 7;
			int lastInd = 0;
			while (lastInd < ipBufSize - 1 && *ip != '\0') {
				if (*ip == '\"') break;
				ipBuf[lastInd++] = *ip;
				ip++;
			}
			ipBuf[lastInd] = '\0';
		}
	}
	return ret;
}

// Send an AT command to the Wemos and show the exchange on the PC terminal
char atResponse[1024];
int showATCommand(char atCMD[]) {
	char line[128];
	snprintf(line, sizeof(line), ">> %s", atCMD);
	sendUART(&huart2, line);
	int ret = sendATCommand(&huart1, atCMD, atResponse, sizeof(atResponse));
	char *start = atResponse;
	char *end;
	while ((end = strstr(start, "\r\n")) != NULL) {
		*end = '\0';
		snprintf(line, sizeof(line), "<< %.120s", start);
		sendUART(&huart2, line);
		start = end + 2;
	}
	return ret;
}

int main(void)
{
  // Init STM32
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();


  //Code Begins
  char buf[32];
  sendUART(&huart2, "STM32-WiFi MODEM TEST");
  HAL_Delay(2000);
  __HAL_UART_CLEAR_OREFLAG(&huart1);
  sendATCommand(&huart1, "ATE0", buf, sizeof(buf));
  // Check modem response
  showATCommand("AT");
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

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
