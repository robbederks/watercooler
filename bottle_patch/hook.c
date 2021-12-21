// This is a patch to have a predefined amount of cold water
// dispensed when you press the "room temp water" button
// while holding the "cold temp water" button

// It hooks into the `dispense_room_temp_water()` function (since that's called first)
// and loops there for a predefined delay if there's also cold water dispensing.

#include <xc.h>
#include <stdint.h>
#include <proc/pic18f6527.h>

// Used pointers in the original code:
volatile uint8_t *return_val_ptr = (volatile uint8_t *)0x0000;
volatile uint8_t *beep_time_L_ptr = (volatile uint8_t *)0x00be;
volatile uint8_t *beep_time_H_ptr = (volatile uint8_t *)0x00bf;

extern void read_port_b_e(void) __at(0x67d2);
extern void beep(void) __at(0x7118);

// Globals
uint32_t fake_dispensing_counter;
uint8_t handsfree;

#define INIT_MAGIC 0xA5A5
volatile uint16_t init_checker;

// Put the patch function at a known empty address
// to make it easy to find in the binary
void patch() __at(0x8000);
void patch() {
  // We wrap the button readout function to be able to fake button presses
  // in the dispensing logic.

  // Init variables if not done so already.
  // Since this is in RAM, everything will have an unknown value at startup
  if (init_checker != INIT_MAGIC) {
    fake_dispensing_counter = 0;
    handsfree = 0;
    init_checker = INIT_MAGIC;
  }

  // Read real status
  read_port_b_e();

  if (fake_dispensing_counter == 0 && ((*return_val_ptr) & 0x3) == 0x3) {
    // Pressing both buttons, so we start the fake dispense and beep.
    // This will dispense about 1L
    fake_dispensing_counter = 0x20000;
    handsfree = 0U;

    *beep_time_L_ptr = (uint8_t) 0x05;
    *beep_time_H_ptr = (uint8_t) 0x01;
    beep();
  }

  if (fake_dispensing_counter > 0) {
    if ((handsfree == 0) && (((*return_val_ptr) & 0x3) == 0)) {
      // We released the buttons
      handsfree = 1U;
    }

    if ((handsfree != 0U) && (((*return_val_ptr) & 0x3) != 0)) {
      // We cancelled by pressing a button
      *beep_time_L_ptr = (uint8_t) 0x05;
      *beep_time_H_ptr = (uint8_t) 0x00;
      beep();

      fake_dispensing_counter = 0U;
      return;
    }

    // Fake only cold water button
    *return_val_ptr = ((*return_val_ptr & 0xf0) | (1 << 1));

    fake_dispensing_counter--;
  }
}

// xc8 does not generate .o files, so we need to have main()
void main() __at(0x10);
void main() {
  patch();
}
