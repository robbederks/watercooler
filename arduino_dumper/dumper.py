#!/usr/bin/env python3
import serial
import binascii

def write_file(data_str, name):
  data = binascii.unhexlify(data_str.decode("utf-8"))
  with open(f"{name}.bin", "wb") as f:
    f.write(data)

if __name__ == "__main__":
  with serial.Serial("/dev/ttyUSB0", 115200) as s:
    print("opened")
    s.write(b"d\n")
    code_data_str = s.readline().strip()
    eeprom_data_str = s.readline().strip()
    config_data_str = s.readline().strip()

  print(len(code_data_str))
  print(len(eeprom_data_str))
  print(len(config_data_str))

  write_file(code_data_str, 'code')
  write_file(eeprom_data_str, 'eeprom')
  write_file(config_data_str, 'config')

  print("Done!")
