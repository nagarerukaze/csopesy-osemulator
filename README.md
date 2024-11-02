# Project Name
MO1 - CSOPESY Emulator


## Overview

This is the Process Scheduler implementation segment of the CSOPESY Emulator.

## Developers
- Jardenil, Aaron Randall **(S11)**
- Jocson, Nicole Pedernal **(S11)**
- Rebano, Jaeme Patrice **(S11)**
- Jaramillo, Brandon Anthony **(S12)**

## Notes

Change "cls" to "clear" in *ConsoleManager* class depending on OS.

```
void clear() {
    ...
    system("cls");      // For Windows
    system("clear")     // For MacOS
    ...
}
```