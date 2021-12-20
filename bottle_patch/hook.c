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
volatile uint8_t *state_flags_ptr = (volatile uint8_t *)0x0011;
volatile uint8_t *input_arg_L_ptr = (volatile uint8_t *)0x00c0;
volatile uint8_t *input_arg_H_ptr = (volatile uint8_t *)0x00c1;

extern void delay(void) __at(0x75c8);
extern void read_port_b_e(void) __at(0x67d2);

#define RETURN_VAL_HOOKED 1U
#define RETURN_VAL_REGULAR 0U

// Put the patch function at a known empty address
// to make it easy to find in the binary
uint8_t patch() __at(0x8000);
uint8_t patch() {
  // Are we dispensing something already?
  if (*state_flags_ptr & (1 << 6)) {
    uint8_t handsfree = 0;

    // Yes -> start the delay!
    for (uint8_t i=0; i<100; i++) {
      *input_arg_L_ptr = (uint8_t) 0x100;
      *input_arg_H_ptr = (uint8_t) 0x0;
      delay();

      // Check that we didn't press a button
      read_port_b_e();
      uint8_t button_pressed = (((*return_val_ptr) & 0x7) != 0);
      if (!handsfree) {
        // We're still waiting on a button release
        if (!button_pressed) {
          handsfree = 1;
        }
      } else {
        // Buttons have been released
        if (button_pressed) {
          // We pressed a button again: cancel!
          break;
        }
      }

      // Kick watchdog
      CLRWDT();
    }

    return RETURN_VAL_HOOKED;
  }

  // If not, we run the code we overwrote and return back into the hooked function
  PORTD |= (1 << 3);

  *input_arg_L_ptr = (uint8_t) 0x9b;
  *input_arg_H_ptr = (uint8_t) 0x0;
  delay();

  return RETURN_VAL_REGULAR;
}

// This is the code we'll replace the beginning of "dispense_room_temp_water" with
uint8_t hook() __at(0x9000);
uint8_t hook() {
  if (patch() == 1U) {
    return 0;
  } else {
    // just an easy instruction to make sure the if is not optimized out
    return 1;
  }
}

// xc8 does not generate .o files, so we need to have main()
void main() __at(0x10);
void main() {
  *input_arg_L_ptr = hook();
}
