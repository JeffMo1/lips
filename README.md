# LIPS
Language Interpreter for Pixel Strips (working name)

# Program structure

One string of 8-bit bytes, up to TBD bytes in length. Instructions contain a minimum of 1 byte and a maximum of TBD bytes.

# Registers

LIPS has a number of registers that provide access to system settings/services, LED RGB data, animation metadata, and scratchpad data for the animation calculations.

_system_

* N - R/O - Number of LEDs in the strip
* S - R/O - Strip identifier, usually sequential from 0 to one less than total number of strips
* R - R/O - Random value (0-255)
* P - R/O - LSB of multiplication operations
* Q - R/O - MSB of multiplication operations

_LED_

* L - R/W - LED index, read for current position, write to change position

_anim_

* F - R/O - Frame counter, read for current frame, auto-incremented by interpreter
* V - R/W - Frame overflow, read for current value, write to change value, affects where frame counter will roll over to zero

_scratch_

* I - R/W - Scratchpad index, read for current position, write to change position
* X - R/W - Scratchpad data, read for current data, write to change data

# Instructions

Code | Name | Parameters | Description
---- | ---- | ---------- | -----------
**Z**  | Terminate frame | | Terminate current render frame and advance, using F and V registers
**I**  | Increment | increment_reg | Add one (with rollover) to the increment_reg
**IX** | Increment scr | | Add one (with rollover) to the scratchpad value (X register)
**IJ** | Increment idx | increment_idx | Add one (with rollover) to the scratchpad value (indexed by increment_idx)
**D**  | Decrement | decrement_reg | Subtract one (with rollover) from the decrement_reg
**DX** | Decrement scr | | Subtract one (with rollover) from the scratchpad value (X register)
**DJ** | Decrement idx | | decrement_idx | Subtract one (with rollover) from the scratchpad value (indexed by decrement_idx)
**M**  | Multiply | basis_reg, factor_reg | Multiply basis_reg x factor_reg, results to P/Q
**ML** | Multiply lit | basis_reg, factor_lit | Multiply basis_reg x factor_lit literal value, results to P/Q
**MX** | Multiply scr | basis_reg | Multiply basis_reg x scratchpad value (X register), results to P/Q
**MJ** | Multiply idx | basis_reg, factor_idx | Multiply basis_reg x scratchpad value (indexed by factor_idx), results to P/Q
**L**  | Load | target_reg, source_reg | Load the source_reg value into the target_reg
**LL** | Load lit | target_reg, source_lit | Load the source_lit value into the target_reg
**LX** | Load scr | target_reg | Load the scratchpad value (X register) into the target_reg
**LJ** | Load idx | target_reg, source_idx | Load the scratchpad value (indexed by source_idx) into the target_reg
**W**  | While | basis_reg, compare_reg | Loop while compare_reg != basis_reg (when equal, jump past corresponding E)
**WL** | While lit | basis_lit, compare_reg | Loop while compare_reg != basis_lit
**WX** | While scr | compare_reg | Loop while compare_reg != scratchpad value (X register)
**WJ** | While idx | basis_idx, compare_reg | Loop while compare_reg != scratchpad value (indexed by source_idx)
**E**  | End while | | End a preceding W, WL, WI, or WJ loop
**PL** | Pixel lit | red_lit, green_lit, blue_lit | Update pixel value (indexed by L) with specified literal values
**PJ** | Pixel idx | red_idx, green_idx, blue_idx | Update pixel value (indexed by L) with specified scratchpad (indexed) values
**QL** | Pixel add lit | red_lit, green_lit, blue_lit | Update pixel value (indexed by L) by adding specified literal values
**QJ** | Pixel add idx | red_idx, green_idx, blue_idx | Update pixel value (indexed by L) by adding specified scratchpad (indexed) values
**R**  | Pixel red | red_reg | Update pixel value (red value only, indexed by L) with red_reg value
**RL** | Pixel red lit | red_lit | Update pixel value (red value only, indexed by L) with red_lit value
**RX** | Pixel red scr | | Update pixel value (red value only, indexed by L) with scratchpad value (X register)
**RJ** | Pixel red idx | red_idx | Update pixel value (red value only, indexed by L) with scratchpad value (indexed by red_idx)
**G**  | Pixel green | green_reg | Update pixel value (green value only, indexed by L) with green_reg value
**GL** | Pixel green lit | green_lit | Update pixel value (green value only, indexed by L) with green_lit value
**GX** | Pixel green scr | | Update pixel value (green value only, indexed by L) with scratchpad value (X register)
**GJ** | Pixel green idx | green_idx | Update pixel value (green value only, indexed by L) with scratchpad value (indexed by green_idx)
**B**  | Pixel blue | blue_reg | Update pixel value (blue value only, indexed by L) with blue_reg value
**BL** | Pixel blue lit | blue_lit | Update pixel value (blue value only, indexed by L) with blue_lit value
**BX** | Pixel blue scr | | Update pixel value (blue value only, indexed by L) with scratchpad value (X registerI)
**BJ** | Pixel blue idx | blue_idx | Update pixel value (blue value only, indexed by L) with scratchpad value (indexed by blue_idx)
