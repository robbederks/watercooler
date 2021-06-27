#!/usr/bin/env python3
import serial
import binascii

if __name__ == "__main__":
  with serial.Serial("/dev/ttyUSB0", 115200) as s:
    print("opened")
    s.write(b"d\n")
    data_str = s.readline().strip()

  print(len(data_str))
  print(data_str)
  data = binascii.unhexlify(data_str.decode("utf-8"))
  with open("dump.bin", "wb") as f:
    f.write(data)
  print("Done!")
