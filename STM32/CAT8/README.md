# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 8

**DATE:** DD/MM/YYYY

**NAME OF EXP:** STM32 GPIO Bring-Up and Switch-Controlled LED

**HARDWARE SETUP / PLATFORM:** CubeMX + STM32CubeIDE + STM32 Development Board Blue Pill STM32F103C6T6A + Push-Button

---

## TASK 8A GPIO Input/Output

### 1. Circuit Diagram

The experiment uses the onboard LED of the STM32 Blue Pill and an external push-button.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C6T6A (ARM Cortex-M3, 72 MHz from the 8 MHz HSE crystal)
- **GPIO Pins:** PC13 (output, push-pull), PA15 (input, internal pull-up)
- **Peripherals:** Onboard LED on PC13 (active-low), push-button between PA15 and GND
- **Power Supply:** 3.3 V from the board; push-button returns to GND

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

PC13 is configured as a push-pull output for the onboard LED and PA15 as an input with the internal pull-up enabled, so the button reads HIGH when released and LOW when pressed. PA15 is normally a JTAG pin, so the debug interface is set to Serial Wire only to free it. The program continuously copies the switch state to the LED, so the LED follows the switch.

**Pseudocode:**

1. Initialize the HAL library and the system clock.
2. Configure PC13 as output and PA15 as input with pull-up.
3. Read the button on PA15.
4. If the button is pressed (LOW), turn the LED ON (PC13 LOW).
5. Otherwise, turn the LED OFF (PC13 HIGH).
6. Repeat continuously.

**Code Base:**

- **Project folder:** [`TASK_8A_GPIO_Input_Output`](TASK_8A_GPIO_Input_Output/)
- `HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)` reads the button.
- `HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET / GPIO_PIN_SET)` turns the LED ON / OFF (active-low).

### 3. Inference

The LED is ON while the button is held and OFF when it is released, demonstrating GPIO input and output configuration with the internal pull-up and active-low LED using STM32 HAL functions.

---

## TASK 8B Event-Based Control

### 1. Circuit Diagram

The same circuit as Task 8A is used.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C6T6A (ARM Cortex-M3, 72 MHz from the 8 MHz HSE crystal)
- **GPIO Pins:** PC13 (output, push-pull), PA15 (input, internal pull-up)
- **Peripherals:** Onboard LED on PC13 (active-low), push-button between PA15 and GND
- **Power Supply:** 3.3 V from the board; push-button returns to GND

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

PA15 is configured as a digital input with the internal pull-up enabled, so it reads HIGH when the button is released and LOW when it is pressed. PA15 is normally a JTAG pin, so the debug interface is set to Serial Wire only to free it. The program detects the press and release edges of the button and toggles the PC13 LED once per valid press. Switch bounce is handled in software: a press must last at least 30 ms before it is accepted, so the short pulses caused by contact bounce are ignored.

**Pseudocode:**

1. Initialize the HAL library and the system clock.
2. Configure PC13 as output and PA15 as input with pull-up.
3. Read the button state and the current time (`HAL_GetTick()`) on every loop.
4. On a press edge (HIGH → LOW), record the press time.
5. On a release edge (LOW → HIGH), calculate the press duration.
6. If the duration is at least 30 ms, toggle the LED; otherwise ignore it as bounce.
7. Store the current state as the previous state and repeat.

**Code Base:**

- **Project folder:** [`TASK_8B_Event_Based_Control`](TASK_8B_Event_Based_Control/)
- `HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)` reads the button.
- `HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)` toggles the LED.
- `HAL_GetTick()` provides the millisecond time stamps for the press and release edges.
- The variables `prev` (previous button state) and `press_t` (press time) are used to detect edges and measure the press.

### 3. Inference

Each press of the button toggles the onboard LED exactly once. Acting only on the release edge and ignoring presses shorter than 30 ms removes the effect of switch bounce, demonstrating GPIO input handling and software debouncing using STM32 HAL functions.

---

## TASK 8C Short Press vs Long Press

### 1. Circuit Diagram

The same circuit as Task 8A is used.

- **Development Board:** STM32 Blue Pill
- **Microcontroller IC:** STM32F103C6T6A
- **GPIO Pins:** PC13 (LED output), PA15 (button input, pull-up)
- **Peripherals:** Onboard LED (active-low), push-button to GND
- **Power Supply:** 3.3 V from the board

![Circuit Diagram](circuit.png)

### 2. Core Logic / Pseudocode / Code Base

**Working Principle:**

The press is classified by how long the button is held, measured without any blocking delay. The press time is recorded on the press edge and the duration is calculated on the release edge. A press shorter than 1 second is a short press and a press of 1 second or more is a long press. A long press switches the LED into continuous blinking mode, where the LED is toggled every 200 ms by comparing `HAL_GetTick()` with the time of the last toggle.

| User action | System response |
|---|---|
| Short press | Toggle LED |
| Long press | LED enters continuous blinking mode |
| Next short press | Stop blinking |
| Long press again | Return to blinking mode |

**Pseudocode:**

1. Initialize the HAL library, clock and GPIO pins.
2. On a press edge, record the start time.
3. Continue executing the application while the button is held.
4. On a release edge, record the release time and calculate `duration = release − start`.
5. If the duration is 1000 ms or more, toggle the blinking mode.
6. Else, if the duration is at least 30 ms:
   - if blinking mode is off, toggle the LED;
   - if blinking mode is on, stop blinking.
7. If blinking mode is on and 200 ms have passed since the last toggle, toggle the LED and record the time.
8. Repeat continuously.

**Code Base:**

- **Project folder:** [`TASK_8C_Short_Press_vs_Long_Press`](TASK_8C_Short_Press_vs_Long_Press/)
- `HAL_GetTick()` is used for both press timing and the blink interval; `HAL_Delay()` is not used.
- `blink` holds the blinking mode, `press_t` the press start time and `blink_t` the last blink toggle time.
- `HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)` is used for both the short-press toggle and the blinking.
- The application code is placed inside the CubeMX `USER CODE` sections of `Core/Src/main.c`.

### 3. Inference

A short press toggles the LED and a long press of 1 second or more starts or stops continuous blinking. Measuring the press duration with `HAL_GetTick()` time stamps instead of a blocking delay keeps the program responsive, so the LED keeps blinking while the button is being read.

---

## My Keil MDK Project

- **Project folder:** [`Led_Toggle`](Led_Toggle/)
- **Board:** Blue Pill STM32F103C8T6, LED PC13 (active-low), switch PA9 (pull-up)
- **Logic:** Short press (< 1 s) toggles the LED. Long press (>= 1 s) starts or stops 200 ms blinking. 30 ms debounce.
- **Circuit Diagram:**

![Circuit Diagram](Led_Toggle/circuit.png)
