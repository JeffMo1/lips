# LIPS
Language Interpreter for Pixel Strips (working name)

# Program structure

One string of 8-bit bytes, up to TBD bytes in length. Instructions contain a minimum of 1 byte and a maximum of TBD bytes.

# Registers

LIPS has a number of registers that provide access to system settings/services, LED RGB data, animation metadata, and scratchpad data for the animation calculations. There are 256 total registers, and certain registers at the end of that address space have special functions, as shown in the table. (Other registers are general purpose.)

_system registers_

Code | Index | Name | Access | Notes
---- | ----- | ---- | ------ | -----
**R_N** | 255 | LED Count | R/O | Number of LEDs in the strip (defined at compile time)
**R_S** | 254 | Strip ID  | R/O | Convention is zero-based, sequential (defined by hardware configuration)
**R_R** | 253 | Random    | R/O | Random byte value on each read, 0-255
**R_P** | 252 | Mult LSB  | R/W | Receives least significant byte of result during multiplications
**R_Q** | 251 | Mult MSB  | R/W | Receives most significant byte of result during multiplications

_LED register_


Code | Index | Name | Access | Notes
---- | ----- | ---- | ------ | -----
**R_L** | 250 | LED Index | R/W | Current working LED index (0..**R_N**-1), used for **P**, **Q**, **R**, **G**, **B** instruction families

_animation registers_

Code | Index | Name | Access | Notes
---- | ----- | ---- | ------ | -----
**R_F** | 249 | Frame Counter  | R/O | Current working frame, initially zero, auto-incremented by interpreter after each frame
**R_V** | 248 | Frame Overflow | R/W | After each frame, frame counter is reset to zero if >= this overflow value


_scratchpad registers_

Code | Index | Name | Access | Notes
---- | ----- | ---- | ------ | -----
**R_I** | 247 | Scratchpad Index | R/W | Current working index into scratchpad data
**R_X** | 246 | Scratchpad Data  | R/W | Read or write the byte at current indexed scratchpad location

# Instructions

Code | Name | Parameters | Description
---- | ---- | ---------- | -----------
**Z**  | Terminate frame | | Terminate current render frame and advance, using F and V registers
**I**  | Increment | increment_reg | Add one (with rollover) to the increment_reg
**IX** | Increment scr | | Add one (with rollover) to the scratchpad value (R_X)
**IJ** | Increment idx | increment_idx | Add one (with rollover) to the scratchpad value (indexed by increment_idx)
**D**  | Decrement | decrement_reg | Subtract one (with rollover) from the decrement_reg
**DX** | Decrement scr | | Subtract one (with rollover) from the scratchpad value (R_X)
**DJ** | Decrement idx | decrement_idx | Subtract one (with rollover) from the scratchpad value (indexed by decrement_idx)
**M**  | Multiply | basis_reg, factor_reg | Multiply basis_reg x factor_reg, results to P/Q
**ML** | Multiply lit | basis_reg, factor_lit | Multiply basis_reg x factor_lit literal value, results to P/Q
**MX** | Multiply scr | basis_reg | Multiply basis_reg x scratchpad value (R_X), results to P/Q
**MJ** | Multiply idx | basis_reg, factor_idx | Multiply basis_reg x scratchpad value (indexed by factor_idx), results to P/Q
**L**  | Load | target_reg, source_reg | Load the source_reg value into the target_reg
**LL** | Load lit | target_reg, source_lit | Load the source_lit value into the target_reg
**LX** | Load scr | target_reg | Load the scratchpad value (R_X) into the target_reg
**LJ** | Load idx | target_reg, source_idx | Load the scratchpad value (indexed by source_idx) into the target_reg
**W**  | While | compare_reg, basis_reg | Loop while compare_reg != basis_reg (when equal, jump past corresponding E)
**WL** | While lit | compare_reg, basis_lit | Loop while compare_reg != basis_lit
**WX** | While scr | compare_reg | Loop while compare_reg != scratchpad value (R_X)
**WJ** | While idx | compare_reg, basis_idx | Loop while compare_reg != scratchpad value (indexed by basis_idx)
**E**  | End while | | End a preceding W, WL, WI, or WJ loop
**PL** | Pixel lit | red_lit, green_lit, blue_lit | Update LED &#x1F534;&#x1F7E2;&#x1F535; at position R_L with specified literal values
**PJ** | Pixel idx | red_idx, green_idx, blue_idx | Update LED &#x1F534;&#x1F7E2;&#x1F535; at position R_L with specified scratchpad (indexed) values
**QL** | Pixel add lit | red_lit, green_lit, blue_lit | Update LED &#x1F534;&#x1F7E2;&#x1F535; at position R_L by adding specified literal values
**QJ** | Pixel add idx | red_idx, green_idx, blue_idx | Update LED &#x1F534;&#x1F7E2;&#x1F535; at position R_L by adding specified scratchpad (indexed) values
**R**  | Pixel red | red_reg | Update LED &#x1F534; at position R_L with red_reg value
**RL** | Pixel red lit | red_lit | Update LED &#x1F534; at position R_L with red_lit value
**RX** | Pixel red scr | | Update LED &#x1F534; at position R_L with scratchpad value (R_X)
**RJ** | Pixel red idx | red_idx | Update LED &#x1F534; at position R_L with scratchpad value (indexed by red_idx)
**G**  | Pixel green | green_reg | Update LED &#x1F7E2; at position R_L with green_reg value
**GL** | Pixel green lit | green_lit | Update LED &#x1F7E2; at position R_L with green_lit value
**GX** | Pixel green scr | | Update LED &#x1F7E2; at position R_L with scratchpad value (R_X)
**GJ** | Pixel green idx | green_idx | Update LED &#x1F7E2; at position R_L with scratchpad value (indexed by green_idx)
**B**  | Pixel blue | blue_reg | Update LED &#x1F535; at position R_L with blue_reg value
**BL** | Pixel blue lit | blue_lit | Update LED &#x1F535; at position R_L with blue_lit value
**BX** | Pixel blue scr | | Update LED &#x1F535; at position R_L with scratchpad value (R_X)
**BJ** | Pixel blue idx | blue_idx | Update LED &#x1F535; at position R_L with scratchpad value (indexed by blue_idx)
