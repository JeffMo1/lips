// LED definitions

#include <FastLED.h>
#define NUM_LEDS 150
#define DATA_PIN 12

CRGB leds[NUM_LEDS];

// Interpreter globals
// https://github.com/JeffMo1/lips/#program-structure

#include "comparisons.h"
#include "instructions.h"
#include "modes.h"
#include "registers.h"

byte instruction;
byte instruction_length;
word iptr;
word yptr;
word lptr;

word stack[16];
byte sptr;

byte mode;

byte registers[256];
byte scratchpad[256];

byte instructions[512] = {
  LL, R_D, 5,
  LL, R_O, 50,
  LL, 0, 0,    // use 0 for hue A
  LL, 1, 128,  // use 1 for hue B
  LL, 2, 0,    // use 2 for hue A counter
  LL, 3, 0,    // use 3 for hue B counter
               // use 4 for even/odd check (hue A or hue B)
  LL, 5, 255,  // use 5 for static sat
               // use 6 for local calc (including val)
  LL, 7, 6,    // use 7 for pixel train position (6 to 150)
               // use 8 for toggling train direction
               // use 9 for calculating train arithmetic
  LL, 10, 36,  // use 10 for train R
  LL, 11, 18,  // use 11 for train G
  LL, 12, 0,   // use 12 for train B
  LL, R_V, 60, // random numbers 0-59
  Y,
  L, R_L, R_N,
  W, R_L, R_Z,
    DC, R_L,
    L, 6, R_R,
    DML, 6, 30,
    AL, 6, 48,
    L, 4, R_L,
    DML, 4, 2,
    IFL, 4, 0,
      PH, 0, 5, 6,
    EI,
    IFL, 4, 1,
      PH, 1, 5, 6,
    EI,
    IFO, OP_LE, R_L, 7,
      L, 9, 7,
      AL, 9, 250,
      IFO, OP_GE, R_L, 9,
        P, 10, 11, 12,
      EI,
    EI,
  EW,
  IC, 2,
  DML, 2, 5,
  IFL, 2, 0,
    AL, 0, 2,
    L, 6, R_R,
    DML, 6, 12,
    IFL, 6, 0,
      AL, 0, 5,
    EI,
  EI,
  IC, 3,
  DML, 3, 3,
  IFL, 3, 0,
    AL, 1, 3,
    L, 6, R_R,
    DML, 6, 10,
    IFL, R_R, 0,
      AL, 1, 9,
    EI,
  EI,
  IFL, 8, 0,
    IC, 7,
    IF, 7, R_N,
      LL, 8, 1,
    EI,
    Z,
  EI,
  IFL, 8, 1,
    DC, 7,
    IFL, 7, 6,
      LL, 8, 0,
    EI,
  EI,
  Z
};

void setup() {
  // Initialize FastLED

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  init_env();
  mode = MODE_RUN;
  
  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready - Leonardo
  Serial.println("LIPS");
  dump_program();
}

void init_env() {
  iptr = 0;
  yptr = 0;
  sptr = 0;
  do {
    registers[iptr] = 0;
    scratchpad[iptr] = 0;
  } while (iptr++ != 256);
  init_system();
  init_frame();
}

void dump_program() {
  Serial.print("Z");
  for (int x = 0; x < 230; x++) {
    if ((x % 16) == 0) Serial.println();
    Serial.print(instructions[x] / 100);
    Serial.print((instructions[x] / 10) % 10);
    Serial.print(instructions[x] % 10);
  }
  Serial.println("999");
}

void ins(byte data, int ms) {
  for (int x = 0; x < 8; x++) {
    leds[x] = bitRead(data, x) ? CRGB::Red : CRGB::Black;
  }
  FastLED.show();
  delay(ms);
  for (int x = 0; x < 8; x++) {
    leds[x] = CRGB::Black;
  }
  FastLED.show();
}

void param(byte data, int ms) {
  for (int x = 0; x < 8; x++) {
    leds[x] = bitRead(data, x) ? CRGB::Green : CRGB::Black;
  }
  FastLED.show();
  delay(ms);
  for (int x = 0; x < 8; x++) {
    leds[x] = CRGB::Black;
  }
  FastLED.show();
}

void check_for_register_bytes() {
  if (Serial.available() > 5) {
    byte digit1 = Serial.read() - 48;
    byte digit2 = Serial.read() - 48;
    byte digit3 = Serial.read() - 48;
    byte index = digit1 * 100 + digit2 * 10 + digit3;
    digit1 = Serial.read() - 48;
    digit2 = Serial.read() - 48;
    digit3 = Serial.read() - 48;
    registers[index] = digit1 * 100 + digit2 * 10 + digit3;
    mode = MODE_RUN;
  }
}

void check_for_program_bytes() {
  if (Serial.available() > 2) {
    byte digit1 = Serial.read() - 48;
    byte digit2 = Serial.read() - 48;
    byte digit3 = Serial.read() - 48;

    if (digit1 == 9 && digit2 == 9 && digit3 == 9) {
      dump_program();
      init_env();
      mode = MODE_RUN;
    }
    else {
      instructions[lptr] = digit1 * 100 + digit2 * 10 + digit3;
      lptr++;
    }
  }
}

void check_for_input() {
  if (Serial.available() > 0) {
    if (mode == MODE_RUN) {
      byte incomingByte = Serial.read();
      switch (incomingByte) {
        case 'Z':
          mode = MODE_LOAD_PROGRAM;
          lptr = 0;
          break;
        case 'R':
          mode = MODE_LOAD_REGISTER;
          break;
        default:
          break;
      }
    }
    else if (mode == MODE_LOAD_PROGRAM) {
      check_for_program_bytes();
    }
    else if (mode == MODE_LOAD_REGISTER) {
      check_for_register_bytes();
    }
  }
}

void loop() {
  check_for_input();
  if ((mode == MODE_RUN) || (mode == MODE_LOAD_REGISTER)) { main_interpreter_loop(); }
}

void main_interpreter_loop() {
  iptr = yptr;
  sptr = 0;
  load_instruction();
  bool debug = false;
  bool term = false;

  do {
    //ins(instruction, 1500);
    switch (instruction) {
      case IC:
        if (debug) { Serial.println("IC"); }
        inc_reg(); break;
      case ICX:
        inc_scr(); break;
      case ICJ:
        inc_idx(); break;
      case DC:
        if (debug) { Serial.println("DC"); }
        dec_reg(); break;
      case DCX: dec_scr(); break;
      case DCJ: dec_idx(); break;
      case A:  add_reg(); break;
      case AL:
        if (debug) { Serial.println("AL"); }
        add_lit(); break;
      case AX: add_scr(); break;
      case AJ: add_idx(); break;
      case M:  mult_reg(); break;
      case ML: mult_lit(); break;
      case MX: mult_scr(); break;
      case MJ: mult_idx(); break;
      case DV:  div_reg(); break;
      case DVL: div_lit(); break;
      case DVX: div_scr(); break;
      case DVJ: div_idx(); break;
      case DM:
        if (debug) { Serial.println("DM"); }
        mod_reg(); break;
      case DML:
        if (debug) { Serial.println("DML"); }
        mod_lit(); break;
      case DMX: //mod_scr(); break;
      case DMJ: //mod_idx(); break;
      case L:
        if (debug) { Serial.println("L"); }
        load_reg(); break;
      case LL:
        if (debug) { Serial.println("LL"); }
        load_lit(); break;
      case LX: load_scr(); break;
      case LJ: load_idx(); break;
      case IF:  if_reg(); break;
      case IFL: if_lit(); break;
      case IFJ: //if_idx(); break;
      case IFO: if_op_reg(); break;
      case IFOL: //if_op_lit(); break;
      case IFOJ: //if_op_idx(); break;
      case EI: end_if(); break;
      case W:
        if (debug) { Serial.println("W"); }
        while_reg(); break;
      case WL: while_lit(); break;
      case WX: while_scr(); break;
      case WJ: while_idx(); break;
      case EW:
        if (debug) { Serial.println("EW"); }
        end_while(); break;
      case P:
        if (debug) { Serial.println("P"); }
        pixel_reg(); break;
      case PB: pixel_blk(); break;
      case PL: pixel_lit(); break;
      case PJ: pixel_idx(); break;
      case PH: pixel_hsv_reg(); break;
      case Q:  pixel_add_reg(); break;
      case QL: pixel_add_lit(); break;
      case QJ: pixel_add_idx(); break;
      case R:  pixel_red_reg(); break;
      case RL: pixel_red_lit(); break;
      case RX: pixel_red_scr(); break;
      case RJ: pixel_red_idx(); break;
      case G:  pixel_green_reg(); break;
      case GL: pixel_green_lit(); break;
      case GX: pixel_green_scr(); break;
      case GJ: pixel_green_idx(); break;
      case B:  pixel_blue_reg(); break;
      case BL: pixel_blue_lit(); break;
      case BX: pixel_blue_scr(); break;
      case BJ: pixel_blue_idx(); break;
      case Y:
        if (debug) { Serial.println("Y"); }
        set_yptr(); break;
      default:
        if (debug) { Serial.println("Z"); }
        terminate_frame(); term = true; break;   // Includes Z instruction and any unknown instruction.
    }
  } while (!term);

  advance_frame();
  delay(registers[R_D]); // Wait in milliseconds, after frame termination (display)
}

/*
** Register management
*/

void init_frame() {
  registers[R_F] = 0;
  registers[R_O] = 16;
  registers[R_D] = 50;
}

void advance_frame() {
  registers[R_F]++;
  check_frame_overflow();
}

void check_frame_overflow() {
  if (registers[R_F] >= registers[R_O]) {
    registers[R_F] = 0;
  }
}

void init_system() {
  registers[R_N] = NUM_LEDS;
  registers[R_S] = 0;    // TBD: Read this from hardware pins
  registers[R_R] = random(256);
  registers[R_U] = 0;
  registers[R_V] = 255;
  registers[R_P] = 0;
  registers[R_Q] = 0;
  registers[R_Z] = 0;
}

byte reg_read(byte reg) {
  byte data;

  switch (reg) {
    case R_R: data = registers[R_R]; registers[R_R] = random(registers[R_U], registers[R_V] + 1); break;
    case R_X: data = scratchpad[R_I]; break;
    case R_Z: data = 0; break;
    default:  data = registers[reg];
  }
  return data;
}

void reg_write(byte reg, byte data) {
  switch (reg) {
    case R_N: break;
    case R_S: break;
    case R_R: break;
    case R_F: break;
    case R_Z: break;
    case R_X: scratchpad[R_I] = data; break;
    default:  registers[reg] = data;
  }
}

void push_ip() {
  stack[sptr] = iptr;
  sptr++;
}

void pop_ip() {
  iptr = stack[--sptr];
  load_instruction();
}

void next_ip() {
  iptr += instruction_length;
  load_instruction();
}

void load_instruction() {
  instruction = instructions[iptr];
  instruction_length = i_lengths[instruction];
}

/* Instruction implementations (render)
**
*/

void terminate_frame() {
  FastLED.show();
}

/*
** Instruction implementations (calc)
*/

void inc_reg() {
  reg_write(instructions[iptr + 1], reg_read(instructions[iptr + 1]) + 1);
  next_ip();
}

void inc_scr() {
  reg_write(R_X, reg_read(R_X) + 1);
  next_ip();
}

void inc_idx() {
  scratchpad[instructions[iptr + 1]] += 1;
  next_ip();
}

void dec_reg() {
  reg_write(instructions[iptr + 1], reg_read(instructions[iptr + 1]) - 1);
  next_ip();
}

void dec_scr() {
  reg_write(R_X, reg_read(R_X) - 1);
  next_ip();
}

void dec_idx() {
  scratchpad[instructions[iptr + 1]] -= 1;
  next_ip();
}

void add_reg() {
  byte basis = reg_read(instructions[iptr + 1]);
  reg_write(instructions[iptr + 1], basis + reg_read(instructions[iptr + 2]));
  next_ip();
}

void add_lit() {
  byte basis = reg_read(instructions[iptr + 1]);
  reg_write(instructions[iptr + 1], basis + instructions[iptr + 2]);
  next_ip();
}

void add_scr() {
  byte basis = reg_read(instructions[iptr + 1]);
  reg_write(instructions[iptr + 1], basis + reg_read(R_X));
  next_ip();
}

void add_idx() {
  byte basis = reg_read(instructions[iptr + 1]);
  reg_write(instructions[iptr + 1], basis + scratchpad[instructions[iptr + 2]]);
  next_ip();
}

void mult_reg() {
  word product = reg_read(instructions[iptr + 1]) * reg_read(instructions[iptr + 2]);
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  next_ip();
}

void mult_lit() {
  word product = reg_read(instructions[iptr + 1]) * instructions[iptr + 2];
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  next_ip();
}

void mult_scr() {
  word product = reg_read(instructions[iptr + 1]) * reg_read(R_X);
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  next_ip();
}

void mult_idx() {
  word product = reg_read(instructions[iptr + 1]) * scratchpad[instructions[iptr + 2]];
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  next_ip();
}

void div_reg() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = reg_read(instructions[iptr + 2]);
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
  next_ip();
}

void div_lit() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = instructions[iptr + 2];
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
  next_ip();
}

void div_scr() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = reg_read(R_X);
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
  next_ip();
}

void div_idx() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = scratchpad[instructions[iptr + 2]];
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
  next_ip();
}

void mod_reg() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = reg_read(instructions[iptr + 2]);
  reg_write(instructions[iptr + 1], basis % divisor);
  next_ip();
}

void mod_lit() {
  byte basis = reg_read(instructions[iptr + 1]);
  byte divisor = instructions[iptr + 2];
  reg_write(instructions[iptr + 1], basis % divisor);
  next_ip();
}

void load_reg() {
  reg_write(instructions[iptr + 1], reg_read(instructions[iptr + 2]));
  next_ip();
}

void load_lit() {
  reg_write(instructions[iptr + 1], instructions[iptr + 2]);
  next_ip();
}

void load_scr() {
  reg_write(instructions[iptr + 1], reg_read(R_X));
  next_ip();
}

void load_idx() {
  reg_write(instructions[iptr + 1], scratchpad[instructions[iptr + 2]]);
  next_ip();
}

/*
** Instruction implementations (conditionals and looping)
*/

void skip_past_ei() {
  byte nest_ct = 0;
  do {
    switch(instruction) {
      case IF:
      case IFL:
      case IFJ:
      case IFO:
      case IFOL:
      case IFOJ:
        nest_ct++;
        break;
      case EI: nest_ct--; break;
    }
    next_ip();
  } while (nest_ct > 0);
}

void skip_past_ew() {
  byte nest_ct = 0;
  do {
    switch (instruction) {
      case W: nest_ct++; break;
      case WL: nest_ct++; break;
      case WX: nest_ct++; break;
      case WJ: nest_ct++; break;
      case EW: nest_ct--; break;
    }
    next_ip();
  } while (nest_ct > 0);
}

void if_reg() {
  if (reg_read(instructions[iptr + 1]) == reg_read(instructions[iptr + 2])) {
    next_ip();
  }
  else {
    skip_past_ei();
  }
}

void if_lit() {
  if (reg_read(instructions[iptr + 1]) == instructions[iptr + 2]) {
    next_ip();
  }
  else {
    skip_past_ei();
  }
}

bool if_comparison(byte op, byte compare, byte basis) {
  bool comparison;
  
  switch (op) {
    case OP_EQ: comparison = (compare == basis); break;
    case OP_NE: comparison = (compare != basis); break;
    case OP_LE: comparison = (compare <= basis); break;
    case OP_LT: comparison = (compare < basis); break;
    case OP_GE: comparison = (compare >= basis); break;
    case OP_GT: comparison = (compare > basis); break;
    
    default:
      comparison = false;
  }
  return comparison;
}

void if_op_reg() {
  byte op = instructions[iptr + 1];
  byte compare = reg_read(instructions[iptr + 2]);
  byte basis = reg_read(instructions[iptr + 3]);

  if (if_comparison(op, compare, basis)) {
    next_ip();
  }
  else {
    skip_past_ei();
  }
}

void if_op_lit() {
  byte op = instructions[iptr + 1];
  byte compare = reg_read(instructions[iptr + 2]);
  byte basis = instructions[iptr + 3];

  if (if_comparison(op, compare, basis)) {
    next_ip();
  }
  else {
    skip_past_ei();
  }
}
void end_if() {
  next_ip();
}

void while_reg() {
  if (reg_read(instructions[iptr + 1]) == reg_read(instructions[iptr + 2])) {
    skip_past_ew();
  }
  else {
    push_ip();
    next_ip();
  }
}

void while_lit() {
  if (reg_read(instructions[iptr + 1]) == instructions[iptr + 2]) {
    skip_past_ew();
  }
  else {
    push_ip();
    next_ip();
  }
}

void while_scr() {
  if (reg_read(instructions[iptr + 1]) == reg_read(R_X)) {
    skip_past_ew();
  }
  else {
    push_ip();
    next_ip();
  }
}

void while_idx() {
  if (reg_read(instructions[iptr + 1]) == scratchpad[instructions[iptr + 2]]) {
    skip_past_ew();
  }
  else {
    push_ip();
    next_ip();
  }
}

void end_while() {
  pop_ip();
}

/*
** Instruction implementations (pixel)
*/

void pixel_reg() {
  leds[reg_read(R_L)].setRGB(
    reg_read(instructions[iptr + 1]),
    reg_read(instructions[iptr + 2]),
    reg_read(instructions[iptr + 3])
  );
  next_ip();
}

void pixel_blk() {
  leds[reg_read(R_L)].setRGB(0, 0, 0);
  next_ip();
}

void pixel_lit() {
  leds[reg_read(R_L)].setRGB(
    instructions[iptr + 1],
    instructions[iptr + 2],
    instructions[iptr + 3]
  );
  next_ip();
}

void pixel_idx() {
  leds[reg_read(R_L)].setRGB(
    scratchpad[instructions[iptr + 1]],
    scratchpad[instructions[iptr + 2]],
    scratchpad[instructions[iptr + 3]]
  );
  next_ip();
}

void pixel_hsv_reg() {
  leds[reg_read(R_L)].setHSV(
    reg_read(instructions[iptr + 1]),
    reg_read(instructions[iptr + 2]),
    reg_read(instructions[iptr + 3])
  );
  next_ip();
}

void pixel_add_reg() {
  // red_delta = reg_read(instructions[iptr+1]);
  // green_delta = reg_read(instructions[iptr+2]);
  // blue_delta = reg_read(instructions[iptr+3]);
  next_ip();
}

void pixel_add_lit() {
  // red_delta = instructions[iptr+1];
  // green_delta = instructions[iptr+2];
  // blue_delta = instructions[iptr+3];
  next_ip();
}

void pixel_add_idx() {
  // red_delta = scratchpad[instructions[iptr+1]];
  // green_delta = scratchpad[instructions[iptr+2]];
  // blue_delta = scratchpad[instructions[iptr+3]];
  next_ip();
}

/*
** Instruction implementations (red)
*/

void pixel_red_reg() {
  leds[reg_read(R_L)].r = reg_read(instructions[iptr + 1]);
  next_ip();
}

void pixel_red_lit() {
  leds[reg_read(R_L)].r = instructions[iptr + 1];
  next_ip();
}

void pixel_red_scr() {
  leds[reg_read(R_L)].r = reg_read(R_X);
  next_ip();
}

void pixel_red_idx() {
  leds[reg_read(R_L)].r = scratchpad[instructions[iptr + 1]];
  next_ip();
}

/*
** Instruction implementations (green)
*/

void pixel_green_reg() {
  leds[reg_read(R_L)].g = reg_read(instructions[iptr + 1]);
  next_ip();
}

void pixel_green_lit() {
  leds[reg_read(R_L)].g = instructions[iptr + 1];
  next_ip();
}

void pixel_green_scr() {
  leds[reg_read(R_L)].g = reg_read(R_X);
  next_ip();
}

void pixel_green_idx() {
  leds[reg_read(R_L)].g = scratchpad[instructions[iptr + 1]];
  next_ip();
}

/*
** Instruction implementations (blue)
*/

void pixel_blue_reg() {
  leds[reg_read(R_L)].b = reg_read(instructions[iptr + 1]);
  next_ip();
}

void pixel_blue_lit() {
  leds[reg_read(R_L)].b = instructions[iptr + 1];
  next_ip();
}

void pixel_blue_scr() {
  leds[reg_read(R_L)].b = reg_read(R_X);
  next_ip();
}

void pixel_blue_idx() {
  leds[reg_read(R_L)].b = scratchpad[instructions[iptr + 1]];
  next_ip();
}

/*
** Instruction implementations (miscellaneous)
*/

void set_yptr() {
  next_ip();
  yptr = iptr;
}
