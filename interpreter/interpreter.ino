// LED definitions

#include <FastLED.h>
#define NUM_LEDS 150
#define DATA_PIN 12

CRGB leds[NUM_LEDS];

// https://github.com/JeffMo1/lips/#program-structure

byte instruction;
byte instruction_length;
word iptr;
word yptr;
word lptr;

word stack[16];
byte sptr;

bool run_program;

// https://github.com/JeffMo1/lips/#registers

#include "registers.h"

byte registers[256];
byte scratchpad[256];

// https://github.com/JeffMo1/lips#instructions

const byte Z = 0;

const byte IC = 1;
const byte ICX = 2;
const byte ICJ = 3;

const byte DC = 4;
const byte DCX = 5;
const byte DCJ = 6;

const byte A = 7;
const byte AL = 8;
const byte AX = 9;
const byte AJ = 10;

const byte M = 11;
const byte ML = 12;
const byte MX = 13;
const byte MJ = 14;

const byte DV = 15;
const byte DVL = 16;
const byte DVX = 17;
const byte DVJ = 18;

const byte DM = 19;
const byte DML = 20;
const byte DMX = 21; // tbi
const byte DMJ = 22; // tbi

const byte L = 23;
const byte LL = 24;
const byte LX = 25;
const byte LJ = 26;

const byte I = 27;
const byte IL = 28;
const byte IX = 29; // tbi
const byte IJ = 30; // tbi

const byte U = 31;  // tbi
const byte UL = 32; // tbi
const byte UX = 33; // tbi
const byte UJ = 34; // tbi
const byte EI = 35; // tbi

const byte W = 36;
const byte WL = 37;
const byte WX = 38;
const byte WJ = 39;
const byte EW = 40;

const byte P = 41;
const byte PB = 42;
const byte PL = 43;
const byte PJ = 44;
const byte PH = 45;

const byte Q = 46;
const byte QL = 47;
const byte QJ = 48;

const byte R = 49;
const byte RL = 50;
const byte RX = 51;
const byte RJ = 52;

const byte G = 53;
const byte GL = 54;
const byte GX = 55;
const byte GJ = 56;

const byte B = 57;
const byte BL = 58;
const byte BX = 59;
const byte BJ = 60;

const byte Y = 61;

const byte i_lengths[] = {
  1,                    // Z
  2, 1, 2,              // IC
  2, 1, 2,              // DC
  3, 3, 2, 3,           // A
  3, 3, 2, 3,           // M
  3, 3, 2, 3,           // DV
  3, 3, 2, 3,           // DM
  3, 3, 2, 3,           // L
  3, 3, 2, 3,           // I
  3, 3, 2, 3, 1,        // U
  3, 3, 2, 3, 1,        // W
  4, 1, 4, 4, 4,        // P
  4, 4, 4,              // Q
  2, 2, 1, 2,           // R
  2, 2, 1, 2,           // G
  2, 2, 1, 2,           // B
  1,                    // Y
};


byte instructions[512] = {
  LL, R_D, 5,
  LL, 0, 25,  // spaceship 0
  LL, 1, 125, // spaceship 1
  LL, 2, 0,   // spaceship 0 direction
  LL, 3, 1,   // spaceship 1 direction
  LL, R_V, 50, // random numbers 0-49
  Y,
  L, R_L, R_N,
  W, R_L, R_Z,
    DC, R_L,
    L, 20, R_R,
    DML, 20, 4,
    AL, 20, 12,
    IL, 30, 0,
      P, R_Z, 20, R_Z,
    EI,
    IL, 30, 1,
      P, 20, 20, R_Z,
    EI,
    IL, 30, 2,
      P, 20, R_Z, R_Z,
    EI,
    IL, 30, 3,
      P, 20, R_Z, 20,
    EI,
    IL, 30, 4,
      P, R_Z, R_Z, 20,
    EI,
    IL, 30, 5,
      P, R_Z, 20, 20,
    EI,
    L, 10, 0,
    IC, 10,
    DM, 10, R_N,
    I, R_L, 10,
      PL, 64, 0, 64,
    EI,
    L, 10, 0,
    DC, 10,
    DM, 10, R_N,
    I, R_L, 10,
      PL, 64, 0, 64,
    EI,
    L, 11, 1,
    DC, 11,
    DM, 11, R_N,
    I, R_L, 11,
      PL, 64, 0, 64,
    EI,
    L, 11, 1,
    IC, 11,
    DM, 11, R_N,
    I, R_L, 11,
      PL, 64, 0, 64,
    EI,
    I, R_L, 0,
      PL, 128, 0, 0,
    EI,
    I, R_L, 1,
      PL, 128, 0, 0,
    EI,
    I, 1, 0,
      PL, 128, 128, 128,
    EI,
  EW,
  I, 1, 0,
    IC, 30,
    DML, 30, 6,
  EI,
  IL, R_R, 0,
    IC, 2,
    DML, 2, 2,
  EI,
  IL, 2, 0,
    IC, 0,
  EI,
  IL, 2, 1,
    DC, 0,
  EI,
  DM, 0, R_N,
  IL, R_R, 0,
    IC, 3,
    DML, 3, 2,
  EI,
  IL, 3, 0,
    IC, 1,
  EI,
  IL, 3, 1,
    DC, 1,
  EI,
  DM, 1, R_N,
  Z
};

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

void setup() {
  // Initialize FastLED

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  init_env();
  run_program = true;

  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready - Leonardo
  Serial.println("LIPS");
  dump_program();
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

void check_for_input() {
  if (Serial.available() > 0) {
    if (run_program) {
      byte incomingByte = Serial.read();
      if (incomingByte == 'Z') {
        run_program = false;
        lptr = 0;
      }
    }
    else if (Serial.available() > 2) {
      byte digit1 = Serial.read() - 48;
      byte digit2 = Serial.read() - 48;
      byte digit3 = Serial.read() - 48;

      if (digit1 == 9 && digit2 == 9 && digit3 == 9) {
        dump_program();
        init_env();
        run_program = true;
      }
      else {
        instructions[lptr] = digit1 * 100 + digit2 * 10 + digit3;
        lptr++;
      }
    }
  }
}

void loop() {
  check_for_input();
  if (run_program) { main_interpreter_loop(); }
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
      case I:  if_reg(); break;
      case IL: if_lit(); break;
      case IX: //if_scr(); break;
      case IJ: //if_idx(); break;
      case U:  //unless_reg(); break;
      case UL: //unless_lit(); break;
      case UX: //unless_scr(); break;
      case UJ: //unless_idx(); break;
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
    //delay(250);
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
      case I: nest_ct++; break;
      case IL: nest_ct++; break;
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
void if_less_reg() {
  if (reg_read(instructions[iptr + 1]) < reg_read(instructions[iptr + 2])) {
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
