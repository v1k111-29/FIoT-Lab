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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bme280.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//HTTP Server Related
typedef enum { CLOSED, CONNECTED, DATA } connection_state;
typedef struct {
	connection_state state;
	char data[128];
} connection;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
struct bme280_dev dev;
struct bme280_settings settings;
struct bme280_data comp_data;
int8_t rslt = BME280_OK;

// I2C address of the sensor: 0x76 (SDO low). Change to BME280_I2C_ADDR_SEC (0x77) if SDO is high.
static uint8_t dev_addr = BME280_I2C_ADDR_PRIM;

// Final values -- watch these in the Live Expressions view
float temperature, pressure, humidity;
connection connections[5];
uint8_t byteRecieved;
char request[128];
char tmpbuf[256];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
void user_delay_us(uint32_t period, void *intf_ptr);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// UART Related
void onWifiDisconnect();
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
	char recBuf[512];
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

//HTTP Server Related
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		int ind = strlen(request);
		if ((byteRecieved != '\r' && byteRecieved != '\n') && (ind < (int)sizeof(request)-1)) {
			request[ind++] = byteRecieved;
			request[ind] = '\0';
		} else {
			if (strstr(request,"WIFI DISCONNECT") != NULL) {
				onWifiDisconnect();
			} else if (strstr(request,"CONNECT") != NULL) {
				if (request[0] >= '0' && request[0] <= '4' && request[1] == ',')
					connections[request[0] - '0'].state = CONNECTED;
			} else if (strstr(request,"CLOSED") != NULL) {
				if (request[0] >= '0' && request[0] <= '4' && request[1] == ',')
					connections[request[0] - '0'].state = CLOSED;
			}  else {
				char* req = strstr(request,"+IPD");
				if (req != NULL) {
					req += 5;
					int link = *req - '0';
					req += 2;
					while (*req != ':' && *req != '\0') {
						req++;
					}
					if (link >= 0 && link <= 4 && *req == ':') {
						req += 1;
						if (connections[link].state != DATA) {
							strcpy(connections[link].data,req);
							connections[link].state = DATA;
						}
					}
				}
			}
			request[0] = '\0';
		}
		HAL_UART_Receive_IT(&huart1, &byteRecieved, 1);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		request[0] = '\0';
		HAL_UART_Receive_IT(&huart1, &byteRecieved, 1);
	}
}

int initServer() {
	char outBuf[8];
	int r1 = sendATCommand(&huart1, "AT+CIPMUX=1", outBuf, sizeof(outBuf));
	if (!r1) {
		return r1;
	}
	if (r1) {
		r1 = sendATCommand(&huart1, "AT+CIPSERVER=1,80", outBuf, sizeof(outBuf));
		HAL_UART_Receive_IT(&huart1, &byteRecieved, 1);
	}
	return r1;
}

int sendHTML(char content[],int link) {
	char out[1024];
	char toSend[32];
	int len = snprintf(out,sizeof(out),"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",strlen(content),content);
	if (len >= (int)sizeof(out)) len = sizeof(out) - 1;
	snprintf(toSend,sizeof(toSend),"AT+CIPSEND=%d,%d",link,len);

	HAL_UART_AbortReceive(&huart1);

	int ret = sendATCommand(&huart1, toSend, tmpbuf, sizeof(tmpbuf));
	if (ret) {
		uint8_t c;
		do {
			HAL_UART_Receive(&huart1, &c, 1, HAL_MAX_DELAY);
		} while (c != '>');

		HAL_UART_Transmit(&huart1, (uint8_t *)out, len, HAL_MAX_DELAY);

		char recBuf[64];
		do {
			recieveUART(&huart1, recBuf, sizeof(recBuf));
		} while (strcmp(recBuf, "SEND OK") != 0 && strcmp(recBuf, "SEND FAIL") != 0 && strcmp(recBuf, "ERROR") != 0);
		ret = (strcmp(recBuf, "SEND OK") == 0);
	}

	snprintf(toSend,sizeof(toSend),"AT+CIPCLOSE=%d",link);
	sendATCommand(&huart1, toSend, tmpbuf, sizeof(tmpbuf));
	connections[link].state = CLOSED;

	request[0] = '\0';
	HAL_UART_Receive_IT(&huart1, &byteRecieved, 1);
	return ret;
}

void handleResponse(void (*respHandler)(char*,int)) {
	for (int i = 0;i < 5;i++) {
		if (connections[i].state == DATA) {
			char* url = strstr(connections[i].data,"GET");
			if (url != NULL) {
				url += 4;
				char urlRec[32];
				int lInd = 0;
				while (*url != ' ') {
					urlRec[lInd++] = *url++;
				}
				urlRec[lInd] = '\0';
				respHandler(urlRec,i);
			} else {
				sendHTML("Not found",i);
			}
		}
	}
}

void handleRouting(char url[],int client) {
	char toPrint[32];
	snprintf(toPrint,sizeof(toPrint),"%d: GET %s",client,url);
	sendUART(&huart6, toPrint);
	if (!strcmp(url,"/")) {
		char data[64];
		snprintf(data,sizeof(data),"%f",temperature);
		sendHTML(data,client);
	} else {
		sendHTML("Page not found",client);
	}
}

void onWifiDisconnect() {
	HAL_UART_AbortReceive(&huart1);
	sendUART(&huart6, "Wifi Disconnected. Reconnecting...");
	char data[32];
	data[0] = '\0';
	while (strcmp(data,"WIFI GOT IP")) {
		recieveUART(&huart1, data, sizeof(data));
	}
	sendUART(&huart6, "Connected. Reinitializing Server");
	initServer();
	sendUART(&huart6, "Server initialized successfully");
}
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  // Tell the Bosch driver how to talk to the sensor
  dev.intf_ptr = &dev_addr;
  dev.intf     = BME280_I2C_INTF;
  dev.read     = user_i2c_read;
  dev.write    = user_i2c_write;
  dev.delay_us = user_delay_us;

  // Checks the chip ID and reads the calibration data
  rslt = bme280_init(&dev);
  if (rslt != BME280_OK) Error_Handler();

  // Configure oversampling, filter and standby time
  rslt = bme280_get_sensor_settings(&settings, &dev);
  settings.osr_h        = BME280_OVERSAMPLING_2X;
  settings.osr_p        = BME280_OVERSAMPLING_2X;
  settings.osr_t        = BME280_OVERSAMPLING_2X;
  settings.filter       = BME280_FILTER_COEFF_OFF;

  rslt = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &dev);
  if (rslt != BME280_OK) Error_Handler();

  // Normal mode: the sensor measures continuously on its own
  rslt = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &dev);
  if (rslt != BME280_OK) Error_Handler();

  char buf[32];
  sendUART(&huart6, "Initializing Wifi...");
  initWifi(&huart1);
  sendUART(&huart6, "Initialized Wifi");
  setMode(&huart1, "STATION", buf, sizeof(buf));

  sendUART(&huart6, "Connecting to Wifi...");
  int ret = joinAP(&huart1, "YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD", buf, sizeof(buf));
  if (ret) {
	  sendUART(&huart6, "Connected to Wifi Successfully");
	  char buf[128];
	  char buf2[64];
	  getIP(&huart1, buf2, sizeof(buf2));
	  snprintf(buf,sizeof(buf),"IP Address: %s",buf2);
	  sendUART(&huart6, buf);

	  initServer();

  } else {
	  sendUART(&huart6, "Failed to connect to wifi");
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /*rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);

    if (rslt == BME280_OK)
    {
      temperature = comp_data.temperature;        // deg C
      pressure    = comp_data.pressure / 100.0f;  // Pa -> hPa
      humidity    = comp_data.humidity;           // % RH
    }

    HAL_Delay(1000);  // read once per second*/
	if (ret) {
		handleResponse(handleRouting);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
	    if (rslt == BME280_OK)
	    {
	      temperature = comp_data.temperature;        // deg C
	      pressure    = comp_data.pressure / 100.0f;  // Pa -> hPa
	      humidity    = comp_data.humidity;           // % RH
	    }
		HAL_Delay(2000);
	}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  uint8_t addr = *(uint8_t *)intf_ptr;
  // HAL expects the 7-bit address shifted left by 1
  if (HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT,
                       reg_data, (uint16_t)len, HAL_MAX_DELAY) == HAL_OK)
  {
    return BME280_OK;
  }
  return BME280_E_COMM_FAIL;
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  uint8_t addr = *(uint8_t *)intf_ptr;
  if (HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT,
                        (uint8_t *)reg_data, (uint16_t)len, HAL_MAX_DELAY) == HAL_OK)
  {
    return BME280_OK;
  }
  return BME280_E_COMM_FAIL;
}

void user_delay_us(uint32_t period, void *intf_ptr)
{
  // HAL only has a millisecond delay, so round microseconds up to the next ms
  (void)intf_ptr;
  HAL_Delay((period / 1000) + 1);
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
