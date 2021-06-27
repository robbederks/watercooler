# ION 901 Watercooler

Dumping and reverse engineering repo of the ION 901 watercooler.
The microcontroller is a PIC18F6527

## PCB Connections
### DIP switches
The switches connect the signal to GND when on. When off, the signals are pulled up.

| DIP PIN | PIC PIN |
|---------|---------|
| 1 | RG0 |
| 2 | RG1 |
| 3 | RG2 |
| 4 | RG3 |
| 5 | RF7 |
| 6 | RG4 |
| 7 | RF6 |
| 8 | RF5 |

### Valves
| Valve | PIC PIN |
|-------|---------|
| Inlet | RD0 |
| B | RD1 |
| C | RD2 |
| D | RD3 |
| Fill | RD4 |

### Other
| Part | PIC PIN |
|------|---------|
| Buzzer | RC2 |
| Compressor | RE1 |
| Heater | RE2 |
| Mixer | RE3 |
| Fan | RE0 |