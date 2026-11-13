# TASK 7A LCD Initialization and Character String Display

Task project for CAT7. The experiment record (aim, circuit, logic, inference) is in [`../README.md`](../README.md).

- Keil project: `lcd.uvproj`
- Source code: `lcd.c`
- Startup file: `STARTUP.A51`
- Firmware: `Objects/lcd.hex`, built with SDCC 4.2.0 (`sdcc -mmcs51`) from the code in this folder, because Keil C51 was not available. A Keil C51 build gives a different hex file for the same program.
