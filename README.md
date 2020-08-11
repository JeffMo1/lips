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
**M**  | Move | target_reg, source_reg | Move the source_reg value into the target_reg
**ML** | Move lit | target_reg, source_lit | Move the source_lit value into the target_reg
**MI** | Move scr | target_reg | Move the scratchpad value (indexed by I) into the target_reg
**MJ** | Move idx | target_reg, source_idx | Move the scratchpad value (indexed by source_idx) into the target_reg
**W**  | While | basis_reg, compare_reg | Loop while compare_reg != basis_reg (when equal, jump past corresponding E)
**WL** | While lit | basis_lit, compare_reg | Loop while compare_reg != basis_lit
**WI** | While scr | compare_reg | Loop while compare_reg != scratchpad value (indexed by I)
**WJ** | While idx | basis_idx, compare_reg | Loop while compare_reg != scratchpad value (indexed by source_idx)
**E**  | End while | | End a preceding W, WL, WI, or WJ loop
**PL** | Pixel lit | red_lit, green_lit, blue_lit | Update pixel value (indexed by L) with specified literal values
**PJ** | Pixel idx | red_idx, green_idx, blue_idx | Update pixel value (indexed by L) with specified scratchpad (indexed) values
**QL** | Pixel add lit | red_lit, green_lit, blue_lit | Update pixel value (indexed by L) by adding specified literal values
**QJ** | Pixel add idx | red_idx, green_idx, blue_idx | Update pixel value (indexed by L) by adding specified scratchpad (indexed) values
**R**  | Pixel red | red_reg | Update pixel value (red value only, indexed by L) with red_reg value
**RL** | Pixel red lit | red_lit | Update pixel value (red value only, indexed by L) with red_lit value
**RI** | Pixel red scr | | Update pixel value (red value only, indexed by L) with scratchpad value (indexed by I)
**RJ** | Pixel red idx | red_idx | Update pixel value (red value only, indexed by L) with scratchpad value (indexed by red_idx)
**G**  | Pixel green | green_reg | Update pixel value (green value only, indexed by L) with green_reg value
**GL** | Pixel green lit | green_lit | Update pixel value (green value only, indexed by L) with green_lit value
**GI** | Pixel green scr | | Update pixel value (green value only, indexed by L) with scratchpad value (indexed by I)
**GJ** | Pixel green idx | green_idx | Update pixel value (green value only, indexed by L) with scratchpad value (indexed by green_idx)
**B**  | Pixel blue | blue_reg | Update pixel value (blue value only, indexed by L) with blue_reg value
**BL** | Pixel blue lit | blue_lit | Update pixel value (blue value only, indexed by L) with blue_lit value
**BI** | Pixel blue scr | | Update pixel value (blue value only, indexed by L) with scratchpad value (indexed by I)
**BJ** | Pixel blue idx | blue_idx | Update pixel value (blue value only, indexed by L) with scratchpad value (indexed by blue_idx)
