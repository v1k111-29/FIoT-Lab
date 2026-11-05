# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 9

**DATE:** DD/MM/YYYY

**NAME OF EXP:** STM32 UART Command Controlled LED

**HARDWARE SETUP / PLATFORM:** CubeMX + STM32CubeIDE + STM32 Development Board Blue Pill STM32F103C6T6A + USB-to-TTL Serial Converter + PuTTY

---

## TASK 9A Basic UART Communication

### 1. Circuit Diagram

The STM32 communicates with a PC terminal through a USB-to-TTL serial converter.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C6T6A (ARM Cortex-M3, 72 MHz)
- **UART Pins:** PA2 (USART2_TX) → converter RX, PA3 (USART2_RX) ← converter TX
- **Peripherals:** USB-to-TTL (3.3 V) serial converter connected to the PC running PuTTY
- **Power Supply:** 3.3 V from the board; converter GND connected to board GND

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

USART2 is configured in CubeMX for asynchronous mode at a standard serial setting of **115200 baud, 8 data bits, no parity, 1 stop bit** (115200-8-N-1). The program waits up to 1 second for a character from the PC:

- **Transmit a Character:** if nothing is received within 1 second, the character `A` is transmitted, so `A` appears on the terminal repeatedly.
- **Receive and Echo:** if a character is received, the same character is transmitted back, so every character typed in PuTTY is echoed on the terminal.

**Pseudocode:**

1. Initialize the HAL library, system clock, GPIO and USART2.
2. Wait up to 1 second for one character from the PC using `HAL_UART_Receive()`.
3. If a character is received, transmit the same character back.
4. Otherwise, transmit the character `A`.
5. Repeat continuously.

**Code Base:**

- **Project folder:** [`TASK_9A_Basic_UART_Communication`](TASK_9A_Basic_UART_Communication/)
- `HAL_UART_Init()` (called from `MX_USART2_UART_Init()`) initializes USART2 with 115200-8-N-1.
- `HAL_UART_Receive(&huart2, &ch, 1, 1000)` waits up to 1 s for one character.
- `HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY)` sends the received character back (echo).
- `HAL_UART_Transmit(&huart2, &txChar, 1, HAL_MAX_DELAY)` sends `A` when no character arrives.

### 3. Inference

The character `A` appears on the terminal once every second, and every character typed in PuTTY is echoed back, confirming that both the transmit and receive paths of the UART work and that the STM32 and the PC use the same baud rate and frame format.

---

## TASK 9B String-Based Application Logic

### 1. Circuit Diagram

The same UART connection as Task 9A is used, along with the onboard LED.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C6T6A
- **UART Pins:** PA2 (USART2_TX), PA3 (USART2_RX)
- **GPIO Pin:** PC13 (output, push-pull)
- **Peripherals:** Onboard LED on PC13 (active-low), USB-to-TTL converter to PuTTY
- **Power Supply:** 3.3 V from the board

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

The received characters are collected into a buffer to form a command string. When a Carriage Return (`\r`) or Line Feed (`\n`) is received, the command is complete: the buffer is terminated and compared with the known commands, and the matching LED action is performed. The LED starts OFF.

| Command | Action |
|---|---|
| `LED ON` | LED turned ON (PC13 LOW) |
| `LED OFF` | LED turned OFF (PC13 HIGH) |
| `LED TOGGLE` | LED toggled |

**Pseudocode:**

1. Initialize the HAL library, clock, GPIO and USART2; turn the LED OFF.
2. Receive one character and echo it back.
3. If the character is `\r` or `\n` and the buffer is not empty:
   - terminate the buffer with `'\0'`;
   - compare it with `LED ON`, `LED OFF` and `LED TOGGLE` and perform the matching action;
   - clear the buffer.
4. Otherwise, add the character to the buffer.
5. Repeat continuously.

**Code Base:**

- **Project folder:** [`TASK_9B_String_Based_Application_Logic`](TASK_9B_String_Based_Application_Logic/)
- `buf[128]` stores the command and `lastInd` tracks its length.
- `strcmp()` compares the received command with each known command.
- `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET / GPIO_PIN_SET)` turns the LED ON / OFF.
- `HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)` toggles the LED.

**Carriage Return and Line Feed:**

| Test string | Result in PuTTY |
|---|---|
| `"HELLO\r\n"` | `HELLO`, then the cursor moves to the start of the next line |
| `"HELLO\r"` | The cursor returns to the start of the same line, so the next text overwrites `HELLO` |
| `"HELLO\n"` | The cursor moves down one line but stays in the same column |
| `"HELLO\r\nWORLD\r\n"` | `HELLO` and `WORLD` on separate lines |

`\r` returns the cursor to the start of the line and `\n` moves it down one line, so both are needed to start a fresh line.

### 3. Inference

Typing `LED ON`, `LED OFF` or `LED TOGGLE` in PuTTY controls the onboard LED. Collecting characters until CR/LF and comparing the complete string turns the UART into a simple text command interface, demonstrating string handling on top of HAL UART communication.

---

## My Keil MDK Project

- **Project folder:** [`UART`](UART/)
- **Board:** Blue Pill STM32F103C8T6, USART2 PA2/PA3 at 115200, LED PC13
- **Logic:** Sends 'A' at start and echoes every character. Typing LED ON, LED OFF or LED TOGGLE and pressing Enter controls the LED.
- **Circuit Diagram:**

![Circuit Diagram](UART/circuit.png)
