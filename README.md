# ION 901 Watercooler

Dumping and reverse engineering repo of the ION 901 watercooler.
The microcontroller is a PIC18F6527

## PCB Connections
### DIP switches
The switches connect the signal to GND when on. When off, the signals are pulled up.

| DIP PIN | PIC PIN | BIT |
|---------|---------|-----|
| 1 | RG0 | 0 |
| 2 | RG1 | 1 |
| 3 | RG2 | 2 |
| 4 | RG3 | 3 |
| 5 | RF7 | 7 |
| 6 | RG4 | 4 |
| 7 | RF6 | 6 |
| 8 | RF5 | 5 |

### Valves
| Valve | PIC PIN |
|-------|---------|
| Inlet | RD0 |
| B (hot) | RD1 |
| C (cold) | RD2 |
| D (room temp) | RD3 |
| Fill | RD4 |

### Other
| Part | PIC PIN |
|------|---------|
| Buzzer | RC2 |
| Compressor | RE1 |
| Heater | RE2 |
| Mixer | RE3 |
| Fan | RE0 |

## Reverse engineering notes
### `state_flags`
| Bit | Set mask | Clear mask | Description |
|-----|----------|------------|-------------|
| 0 | 0x01 | 0xfe | (Re-)booted without power-on reset |
| 1 | 0x02 | 0xfd | (Re-)booted without watchdog trigger |
| 2 | 0x04 | 0xfb | used but unknown purpose (wrote something to EEPROM) |
| 3 | 0x08 | 0xf7 | Water too hot |
| 4 | 0x10 | 0xef | Fan and compressor running |
| 5 | 0x20 | 0xdf | used but unknown purpose |
| 6 | 0x40 | 0xbf | Dispensing something |
| 7 | 0x80 | 0x7f | Handle compressor interrupt|
