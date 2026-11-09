# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 10

**DATE:** 12/10/2026

**NAME OF EXP:** STM32–Wemos Wi-Fi Modem Interface Using AT Commands

**HARDWARE SETUP / PLATFORM:** CubeMX + STM32CubeIDE + STM32 Development Board Blue Pill STM32F103C8T6 + Wemos D1 Mini (ESP8266) Wi-Fi Module + USB-to-TTL Serial Converter + PuTTY

---

## TASK 9A Establish STM32 ↔ Wemos UART Communication

### 1. Circuit Diagram

The STM32 uses two UARTs: one to send AT commands to the Wemos and one to show the results on the PC terminal.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C8T6 (ARM Cortex-M3, 72 MHz)
- **UART1 Pins (Wemos):** PA9 (USART1_TX) → Wemos RX, PA10 (USART1_RX) ← Wemos TX
- **UART2 Pins (PC):** PA2 (USART2_TX) → converter RX, PA3 (USART2_RX) ← converter TX
- **Peripherals:** Wemos D1 Mini (ESP8266) running AT firmware, USB-to-TTL (3.3 V) converter to PuTTY
- **Power Supply:** Wemos powered from 5 V; STM32, Wemos and converter share a common GND

| UART | Connection | Purpose |
|---|---|---|
| USART1 | STM32 ↔ Wemos | AT commands and responses |
| USART2 | STM32 ↔ PC | Debug / display |

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

Both UARTs are configured at **115200 baud, 8-N-1**, the default rate of the ESP8266 AT firmware. The STM32 sends `AT` followed by CR/LF to the Wemos and reads its reply line by line until the final status line (`OK`, `ERROR` or `FAIL`). The command and the Wemos response are then displayed on the PC terminal through USART2.

**Pseudocode:**

1. Initialize the HAL library, clock, USART1 and USART2.
2. Print `STM32-WiFi MODEM TEST` on the PC terminal.
3. Wait 2 s for the ESP8266 to boot and clear the UART overrun flag.
4. Send `ATE0` to turn off command echo.
5. Print `>> AT` on the PC terminal and send `AT\r\n` to the Wemos.
6. Collect the Wemos response until `OK`, `ERROR` or `FAIL`.
7. Print each response line on the PC terminal as `<< <line>`.

**Code Base:**

- **Project folder:** [`TASK_9A_Establish_STM32_Wemos_UART_Communication`](TASK_9A_Establish_STM32_Wemos_UART_Communication/)
- `sendUART()` transmits a string followed by `\r\n` using `HAL_UART_Transmit()`.
- `recieveUART()` reads characters with `HAL_UART_Receive()` until a complete line ends with CR or LF.
- `sendATCommand()` sends one AT command and collects every response line until `OK` (returns 1) or `ERROR`/`FAIL` (returns 0).
- `showATCommand()` prints `>> <command>`, sends it with `sendATCommand()` and prints every response line as `<< <line>`.

**PC Terminal Output:**

```
STM32-WiFi MODEM TEST
>> AT
<< OK
```

### 3. Inference

The Wemos replies `OK` to the `AT` command, proving that the two boards communicate over UART. Using a separate UART for the PC keeps the debug messages apart from the AT-command traffic.

---

## TASK 9B Explore the ESP8266 AT Command Interface

### 1. Circuit Diagram

The same circuit as Task 9A is used.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C8T6
- **UART Pins:** PA9/PA10 (USART1, Wemos), PA2/PA3 (USART2, PC)
- **Peripherals:** Wemos D1 Mini (ESP8266), USB-to-TTL converter
- **Power Supply:** 5 V to the Wemos, common GND

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

Every AT command is a text line ending in CR/LF, and every reply ends with a status line (`OK` on success, `ERROR` or `FAIL` on failure). The commands needed for each requirement were identified from the ESP8266 AT-command documentation:

| Requirement | Command |
|---|---|
| Check modem response | `AT` |
| Check firmware/version | `AT+GMR` |
| Set Wi-Fi station mode | `AT+CWMODE=1` |
| Scan available networks | `AT+CWLAP` |
| Connect to an AP | `AT+CWJAP="<ssid>","<password>"` |
| Obtain network information | `AT+CIFSR` |
| Establish TCP connection | `AT+CIPSTART="TCP","<host>",<port>` |
| Send application data | `AT+CIPSEND=<length>` |

The STM32 sends the commands for the first four requirements to the Wemos one after another and displays each command and its response on the PC terminal. Joining an AP and reading the network information are carried out in Task 9C.

**Pseudocode:**

1. Initialize the Wemos as in Task 9A (boot delay, `ATE0`).
2. For each command `AT`, `AT+GMR`, `AT+CWMODE=1` and `AT+CWLAP`:
   - print `>> <command>` on the PC terminal;
   - send the command to the Wemos followed by CR/LF;
   - collect the response lines until `OK`, `ERROR` or `FAIL`;
   - print each response line as `<< <line>`.

**Code Base:**

- **Project folder:** [`TASK_9B_Explore_ESP8266_AT_Command_Interface`](TASK_9B_Explore_ESP8266_AT_Command_Interface/)
- `showATCommand(cmd)` prints the command, sends it with `sendATCommand()` and prints the collected response.
- `sendATCommand(huart, cmd, output, size)` implements the request–response loop and is used for every command.
- The response is collected into the buffer `atResponse[1024]` first and printed afterwards, so no characters from the Wemos are missed while the PC terminal is being written.

**PC Terminal Output:**

```
STM32-WiFi MODEM TEST
>> AT
<< OK
>> AT+GMR
<< AT version:...
<< SDK version:...
<< OK
>> AT+CWMODE=1
<< OK
>> AT+CWLAP
<< +CWLAP:(3,"<ssid>",-60,"xx:xx:xx:xx:xx:xx",1,...)
<< OK
```

### 3. Inference

The ESP8266 AT interface follows a simple text request–response protocol: every command ends with CR/LF and every response ends with a status line. A single function that sends a command and waits for the final status line is enough to check the modem, read its firmware version, set station mode and scan the available networks.

---

## TASK 9C Connect Wemos to Wi-Fi

### 1. Circuit Diagram

The same circuit as Task 9A is used.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C8T6
- **UART Pins:** PA9/PA10 (USART1, Wemos), PA2/PA3 (USART2, PC)
- **Peripherals:** Wemos D1 Mini (ESP8266), USB-to-TTL converter, Wi-Fi access point
- **Power Supply:** 5 V to the Wemos, common GND

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

The STM32 configures the ESP8266 as a Wi-Fi station, joins the access point and then queries the network status. The IP and MAC addresses are extracted from the `AT+CIFSR` reply and printed on the PC terminal.

**Pseudocode:**

1. Initialize the Wi-Fi module (Task 9A).
2. Send `AT+CWMODE=1` to configure station mode.
3. Print `Connecting to Wifi...` and send `AT+CWJAP="<ssid>","<password>"`.
4. Wait for the response (`WIFI CONNECTED`, `WIFI GOT IP`, `OK`).
5. If connected:
   - print `Connected to Wifi Successfully`;
   - send `AT+CIFSR`, extract the text after `STAIP,"` and print `IP Address: <ip>`;
   - send `AT+CIFSR`, extract the text after `STAMAC,"` and print `MAC Address: <mac>`.
6. Otherwise, print `Failed to connect to wifi`.

**Code Base:**

- **Project folder:** [`TASK_9C_Connect_Wemos_to_WiFi`](TASK_9C_Connect_Wemos_to_WiFi/)
- `initWifi()` performs the boot delay, `__HAL_UART_CLEAR_OREFLAG()`, `ATE0` and `AT`.
- `joinAP()` builds and sends the `AT+CWJAP` command.
- `getIP()` and `getMAC()` send `AT+CIFSR`, locate the `STAIP` / `STAMAC` tag with `strstr()` and copy the quoted value.

**PC Terminal Output:**

```
Initializing Wifi...
Initialized Wifi
Connecting to Wifi...
Connected to Wifi Successfully
IP Address: xxx.xxx.xxx.xxx
MAC Address: xx:xx:xx:xx:xx:xx
```

### 3. Inference

The ESP8266 joins the Wi-Fi network as a station under the control of the STM32, and the assigned IP and MAC addresses are reported on the PC terminal. This shows that the ESP8266 can act as a Wi-Fi modem for a microcontroller that only needs a UART and AT commands.

---

## My Keil MDK Project

- **Project folder:** [`TASK_10C_Web_Server`](TASK_10C_Web_Server/)
- **Board:** NUCLEO-F401RE, USART1 PA9/PA10 to Wemos, USART6 PC6/PC7 to PuTTY
- **Logic:** Joins Wi-Fi, starts a TCP server on port 80 (AT+CIPMUX=1, AT+CIPSERVER=1,80). On every +IPD request it sends a small HTML page with AT+CIPSEND and closes the connection.
- **Circuit Diagram:**

![Circuit Diagram](TASK_10C_Web_Server/circuit.png)
