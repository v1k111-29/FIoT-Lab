# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 14

**DATE:** DD/MM/YYYY

**NAME OF EXP:** BME280 Sensor Data Publishing over MQTT Using ESP8266

**HARDWARE SETUP / PLATFORM:** STM32CubeMX + Keil MDK + NUCLEO-F401RE (STM32F401RET6) + Bosch BME280 (I2C) + ESP8266 Wi-Fi module (USART1)

---

## TASK : Publish BME280 readings to an MQTT broker

### 1. Circuit Diagram

- **Development Board:** NUCLEO-F401RE
- **Microcontroller IC:** STM32F401RET6
- **I2C pins:** PB6 (I2C1_SCL), PB7 (I2C1_SDA) to the BME280
- **UART pins:** PA9 (USART1_TX), PA10 (USART1_RX) to the ESP8266
- **Power:** BME280 on 3.3 V, common GND

![Circuit Diagram](circuit.png)

> TODO: add `circuit.png` to this folder.

### 2. Core Logic / Pseudocode / Code Base

1. Initialise HAL, clock, GPIO, I2C1 and USART1.
2. `ESP8266_Init` in station mode, then `ESP8266_wifiConnect` (set your own SSID and password in `Core/Src/main.c`).
3. `ESP8266_portConnect("TCP", "broker.emqx.io", "1883")`, then `MQTT_connectBroker`.
4. Check the BME280 with `HAL_I2C_IsDeviceReady`, then configure it through the Bosch API.
5. Loop every 2 s: read temperature, pressure and humidity, format JSON and call `MQTT_publishTopic("stm32/leela", ...)` at QoS0.

Source: `Core/Src/main.c`, `Core/Src/ESPLib.c`, `Core/Src/bme280.c`. Firmware: `Task14.hex`.

### 3. INFERENCE

> TODO: add your observations and conclusion.
