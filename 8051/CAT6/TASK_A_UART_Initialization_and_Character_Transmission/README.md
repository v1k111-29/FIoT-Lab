# TASK A UART Initialization and Character Transmission

Task project for CAT6. The experiment record (aim, circuit, logic, inference) is in [`../README.md`](../README.md).

- Keil project: `uartcommunication.uvproj`
- Source code: `prog1.c`
- Startup file: `STARTUP.A51`
- Firmware: `Objects/uartcommunication.hex`, built with SDCC 4.2.0 (`sdcc -mmcs51`) from the code in this folder, because Keil C51 was not available. A Keil C51 build gives a different hex file for the same program.
