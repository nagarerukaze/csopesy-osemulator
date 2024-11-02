# Project Name
MO1 - CSOPESY Emulator


## Overview

Last updated: 11-02-2024

## Developers
- Jardenil, Aaron Randall **(S11)**
- Jocson, Nicole Pedernal **(S11)**
- Rebano, Jaeme Patrice **(S11)**
- Jaramillo, Brandon Anthony **(S12)**

## Notes

Change "cls" to "clear" in ConsoleManager class depending on OS.

```
void clear() {
    ...
    system("cls");      // For Windows
    system("clear")     // For MacOS
    ...
}
```