# Bottle patching process
## Step 1: Making multiple buttons work
To make multiple buttons work, we need to patch the if-else part of the handling code.
In particular, we patch it to use `BTFSC` and `BRA` instructions instead.

Not sure which code path is actually taken in my particular case, so we patch all of them.

We also have to patch out the `close_all_valves()` call on every button state change to a `NOP`. This isn't needed since it's also called when all buttons are released.

## Step 2: Adding in the hook binary
- Compile the binary with the `build.sh` script
- 

## Step 3: Patching the `dispense_room_temp_water` function to call the hook


