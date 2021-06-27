// Pin mappings for the PIC programming shield.
#define PIN_MCLR        A1      // 0: MCLR is VPP voltage, 1: Reset PIC
#define PIN_ACTIVITY    13      // LED that indicates read/write activity
#define PIN_VDD         2       // Controls the power to the PIC
#define PIN_VDD2        3       // Controls the power to the PIC
#define PIN_CLOCK       4       // Clock pin
#define PIN_DATA        7       // Data pin

#define MCLR_RESET      HIGH    // PIN_MCLR state to reset the PIC
#define MCLR_VPP        LOW     // PIN_MCLR state to apply 13v to MCLR/VPP pin

// All delays are in microseconds.
#define DELAY_SETTLE    1000    // Delay for lines to settle for reset
#define DELAY_TPPDP     1000    // Hold time after raising MCLR
#define DELAY_THLD0     1000    // Hold time after raising VDD

void setup()
{
  // Need a serial link to the host.
  Serial.begin(115200);

  // Hold the PIC in the powered down/reset state until we are ready for it.
  pinMode(PIN_MCLR, OUTPUT);
  pinMode(PIN_VDD, OUTPUT);
  pinMode(PIN_VDD2, OUTPUT);
  digitalWrite(PIN_MCLR, MCLR_RESET);

  digitalWrite(PIN_VDD, HIGH);
  digitalWrite(PIN_VDD2, HIGH);

  // Clock and data are floating until the first PIC command.
  pinMode(PIN_CLOCK, INPUT);
  pinMode(PIN_DATA, INPUT);

  // Turn off the activity LED initially.
  pinMode(PIN_ACTIVITY, OUTPUT);
  digitalWrite(PIN_ACTIVITY, LOW);
}

void loop(){
  if(Serial.available()){
    int ch = Serial.read();
    if(ch == 'd') {
      //Serial.println("Attempting dump...");
      exitProgramMode();

      while(true){
        if(readDeviceId()){
          //Serial.println("Device ID is OK!");
          //Serial.println("Trying to read data...");

          setAddress(0);
          char tmp[2];
          for(uint32_t i=0; i<0xC000; i++){
            sprintf(tmp, "%.2x", readPostIncrement());
            Serial.print(tmp);
          }
          Serial.println();
          break;

        } else {
          //Serial.println("Device ID did not match!");
        }

        exitProgramMode();
      }
    }
  }
}

// Enter high voltage programming mode.
void enterProgramMode() {
  // Lower MCLR, VDD, DATA, and CLOCK initially.  This will put the
  // PIC into the powered-off, reset state just in case.
  digitalWrite(PIN_MCLR, MCLR_RESET);
  digitalWrite(PIN_VDD, HIGH);
  digitalWrite(PIN_VDD2, HIGH);
  digitalWrite(PIN_DATA, LOW);
  digitalWrite(PIN_CLOCK, LOW);

  // Wait for the lines to settle.
  delayMicroseconds(DELAY_SETTLE);

  // Switch DATA and CLOCK into outputs.
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);

  // Raise MCLR, then VDD.
  digitalWrite(PIN_MCLR, MCLR_VPP);
  delayMicroseconds(DELAY_TPPDP);
  digitalWrite(PIN_VDD, LOW);
  digitalWrite(PIN_VDD2, LOW);
  delayMicroseconds(DELAY_THLD0);
}

// Exit programming mode and reset the device.
void exitProgramMode(){

  // Lower MCLR, VDD, DATA, and CLOCK.
  digitalWrite(PIN_MCLR, MCLR_RESET);
  digitalWrite(PIN_VDD, HIGH);
  digitalWrite(PIN_VDD2, HIGH);
  digitalWrite(PIN_DATA, LOW);
  digitalWrite(PIN_CLOCK, LOW);

  // Float the DATA and CLOCK pins.
  pinMode(PIN_DATA, INPUT);
  pinMode(PIN_CLOCK, INPUT);
}

#define CMD_CORE              0b0000
#define CMD_SHIFT_OUT_TABLAT  0b0010
#define CMD_READ              0b1000
#define CMD_READ_POST_INC     0b1001
#define CMD_READ_POST_DEC     0b1010
#define CMD_READ_PRE_INC      0b1011
// don't define write now!

void sendCommand(byte cmd) {
  pinMode(PIN_DATA, OUTPUT);
  for(int i=0; i<4; i++){
    digitalWrite(PIN_CLOCK, HIGH);
    if (cmd & 1)
        digitalWrite(PIN_DATA, HIGH);
    else
        digitalWrite(PIN_DATA, LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(1);
    cmd >>= 1;
  }
}

void sendData(uint16_t data) {
  pinMode(PIN_DATA, OUTPUT);
  for(int i=0; i<16; i++){
    digitalWrite(PIN_CLOCK, HIGH);
    if (data & 1)
        digitalWrite(PIN_DATA, HIGH);
    else
        digitalWrite(PIN_DATA, LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(1);
    data >>= 1;
  }
}

void sendCore(uint16_t data) {
  sendCommand(CMD_CORE);
  sendData(data);
}

void setAddress(uint32_t addr) {
  sendCore(0x0E00 | ((addr >> 16) & 0xFF));
  sendCore(0x6EF8);
  sendCore(0x0E00 | ((addr >> 8) & 0xFF));
  sendCore(0x6EF7);
  sendCore(0x0E00 | ((addr >> 0) & 0xFF));
  sendCore(0x6EF6);
}

uint8_t readByte(){
  // First data byte: send empty
  pinMode(PIN_DATA, OUTPUT);
  for(int i=0; i<8; i++){
    digitalWrite(PIN_CLOCK, HIGH);
    digitalWrite(PIN_DATA, LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(1);
  }

  delayMicroseconds(1);
  pinMode(PIN_DATA, INPUT_PULLUP);

  uint8_t data = 0;
  // Second data byte: read!
  for(int i=0; i<8; i++){
    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(1);
    data >>= 1;
    data |= ((digitalRead(PIN_DATA) & 1) << 7);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(1);
  }

  return data;
}

uint8_t readPostIncrement(){
  sendCommand(CMD_READ_POST_INC);
  return readByte();
}

boolean readDeviceId(){
  exitProgramMode();
  enterProgramMode();

  setAddress(0x3FFFFE);
  uint8_t DEVID1 = readPostIncrement();
  uint8_t DEVID2 = readPostIncrement();

  //Serial.print("DEVID1: 0x"); Serial.println(DEVID1, HEX);
  //Serial.print("DEVID2: 0x"); Serial.println(DEVID2, HEX);

  return ((DEVID2 == 0x13) && ((DEVID1 & 0xF0) == 0x40));
}