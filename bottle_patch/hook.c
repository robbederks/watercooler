// This is a patch to have a predefined amount of cold water
// dispensed when you press the "room temp water" button
// while holding the "cold temp water" button

// It hooks into the `dispense_room_temp_water()` function (since that's called first)
// and loops there for a predefined delay if there's also cold water dispensing.

#include <xc.h>
#include <stdint.h>
#include <proc/pic18f6527.h>

// Used pointers in the original code:
volatile uint8_t *state_flags_ptr = (volatile uint8_t *)0x0011;
volatile uint8_t *input_arg_L_ptr = (volatile uint8_t *)0x00c0;
volatile uint8_t *input_arg_H_ptr = (volatile uint8_t *)0x00c1;

extern void delay(void) __at(0x75c8);

// Put the patch function at a known address
// to make it easy to find in the binary
uint8_t patch() __at(0x1000);
uint8_t patch() {
  // Are we dispensing something already?
  if (*state_flags_ptr & (1 << 6)) {
    // Yes -> start the delay!
    for (uint8_t i=0; i<100; i++) {
      *input_arg_L_ptr = (uint8_t) 0x100;
      *input_arg_H_ptr = (uint8_t) 0x0;
      delay();

      // Kick watchdog
      CLRWDT();
    }

    return 1U;
  }

  // If not, we run the code we overwrote and return back into the hooked function
  PORTD |= (1 << 3);

  *input_arg_L_ptr = (uint8_t) 0x9b;
  *input_arg_H_ptr = (uint8_t) 0x0;
  delay();

  return 0U;
}

// This is the code we'll replace the beginning of "dispense_room_temp_water" with
void hook() {
  if (patch() == 1U) {
    return;
  }
}

// xc8 does not generate .o files, so we need to have main()
void main() __at(0x10);
void main() {
  hook();
}
