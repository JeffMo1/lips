# LIPS
Language Interpreter for Pixel Strips (working name)

# Program structure

One string of 8-bit bytes, up to TBD bytes in length. Instructions contain a minimum of 1 byte and a maximum of TBD bytes.

# Registers

LIPS has a number of registers that provide access to strip configurations (read-only), LED RGB data (read/write), animation metadata, and scrachpad data for the animation calculations. 

_config_

* N - R/O - Number of LEDs in the strip 
* S - R/O - Strip identifier, usually sequential from 0 to one less than total number of strips

_LED_

* L - R/W - LED index, read for current position, write to change position

_anim_

* F - R/O - Frame counter, read for current frame, auto-incremented by interpreter
* V - R/W - Frame overflow, read for current value, write to change value, affects where frame counter will roll over to zero

_scratch_

* I - R/W - Scratchpad index, read for current position, write to change position
* X - R/W - Scratchpad data, read for current data, write to change data

# Instructions

*Z* - no parameters

Default operation, terminates current frame for rendering and advancing to next frame, using F and V registers

*L* - target_register, operation, source_rov

Loads the target_register with the 8-bit value computed based on the operation and source_rov parameters.

*W* - basis_register, comparison, compare_rov

Advances the interpreter past the next corresponding E instruction, if the 8-bit comparison of basis_register and compare_rov fails, using the comparison parameter. 

*P* - red, green, blue

Updates a pixel, based on red/green/blue parameters and the current LED based on the L register

*p* - operation, red, green, blue

Updates a pixel, based on red/green/blue parameters, and the current LED based on the L register, with the operation indicating how to calculate new RGB values from old ones

*R* - red

Similar behavior to the P instruction, updates a pixel's red value only

*r* - operation, red

Similar behavior to the p instruction, updates a pixel's red value only

*G* - green

Similar behavior to the P instruction, updates a pixel's green value only

*g* - operation, green

Similar behavior to the p instruction, updates a pixel's green value only

*B* - blue

Similar behavior to the P instruction, updates a pixel's blue value only

*b* - operation, blue

Similar behavior to the p instruction, updates a pixel's blue value only

