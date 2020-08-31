// LED definitions

#include <FastLED.h>
#define NUM_LEDS 150
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

// https://github.com/JeffMo1/lips/#program-structure

byte instructions[256];
byte iptr;
byte stack[16];
byte sptr;

// https://github.com/JeffMo1/lips/#registers

byte registers[256];

const byte R_N = 255;
const byte R_S = 254;
const byte R_R = 253;
const byte R_P = 252;
const byte R_Q = 251;
const byte R_Z = 250;

const byte R_L = 249;

const byte R_F = 248;
const byte R_V = 247;

const byte R_I = 246;
const byte R_X = 245;

byte scratchpad[256];

// https://github.com/JeffMo1/lips#instructions

const byte Z = 0;

const byte I = 1;
const byte IX = 2;
const byte IJ = 3;

const byte D = 4;
const byte DX = 5;
const byte DJ = 6;

const byte A = 7;
const byte AL = 8;
const byte AX = 9;
const byte AJ = 10;

const byte M = 11;
const byte ML = 12;
const byte MX = 13;
const byte MJ = 14;

const byte L = 15;
const byte LL = 16;
const byte LX = 17;
const byte LJ = 18;

const byte W = 19;
const byte WL = 20;
const byte WX = 21;
const byte WJ = 22;
const byte E = 23;

const byte P = 24;
const byte PL = 25;
const byte PJ = 26;

const byte Q = 27;
const byte QL = 28;
const byte QJ = 29;

const byte R = 30;
const byte RL = 31;
const byte RX = 32;
const byte RJ = 33;

const byte G = 34;
const byte GL = 35;
const byte GX = 36;
const byte GJ = 37;

const byte B = 38;
const byte BL = 39;
const byte BX = 40;
const byte BJ = 41;

const byte i_lengths[] = {
  1,                    // Z
  2, 1, 2,              // I
  2, 1, 2,              // D
  3, 3, 2, 3,           // A
  3, 3, 2, 3,           // M
  3, 3, 2, 3,           // L
  3, 3, 2, 3, 1,        // W
  4, 4, 4,              // P
  4, 4, 4,              // Q
  3, 3, 2, 3,           // R
  3, 3, 2, 3,           // G
  3, 3, 2, 3            // B
};

void setup() {
  // Initialize FastLED

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Initialize IP, SP, and program storage.
  
  iptr = 0;
  sptr = 0;
  do {
    instructions[iptr] = Z;
    registers[iptr] = 0;
    scratchpad[iptr] = 0;
  } while (iptr++ != 0); 

  // Load real program here TBD.
  
  init_frame();
}

void loop() {
  byte instruction;
  
  iptr = 0;
  sptr = 0 ;
  do {
    instruction = instructions[iptr];
    switch (instruction) {
      case I:  inc_reg(); break;
      case IX: inc_scr(); break;
      case IJ: inc_idx(); break;
      case D:  dec_reg(); break;
      case DX: dec_scr(); break;
      case DJ: dec_idx(); break;
      case A:  add_reg(); break;
      case AL: add_lit(); break;
      case AX: add_scr(); break;
      case AJ: add_idx(); break;
      case M:  mult_reg(); break;
      case ML: mult_lit(); break;
      case MX: mult_scr(); break;
      case MJ: mult_idx(); break;
      case L:  load_reg(); break;
      case LL: load_lit(); break;
      case LX: load_scr(); break;
      case LJ: load_idx(); break;
      case W:  while_reg(); break;
      case WL: while_lit(); break;
      case WX: while_scr(); break;
      case WJ: while_idx(); break;
      case E:  end_while(); break;
      case P:  pixel_reg(); break;
      case PL: pixel_lit(); break;
      case PJ: pixel_idx(); break;
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
      default: terminate_frame(); break;   // Includes Z instruction and any unknown instruction.
    }
  } while (instruction != Z);

  advance_frame();
  delay(1000); // possible frame speed adjustment could go here
}

/*
** Register management 
*/

void init_frame() {
  registers[R_F] = 0;
  registers[R_V] = 16;
}

void advance_frame() {
  registers[R_F]++;
  check_frame_overflow();
}

void check_frame_overflow() {
  if (registers[R_F] >= registers[R_V]) { registers[R_V] = 0; }
}

void init_system() {
  registers[R_N] = NUM_LEDS;
  registers[R_S] = 0;    // TBD: Read this from hardware pins
  registers[R_R] = random(256);
  registers[R_P] = 0;
  registers[R_Q] = 0;
  registers[R_Z] = 0;
}

byte reg_read(byte reg) {
  byte value;

  switch (reg) {
    case R_R: value = registers[R_R]; registers[R_R] = random(256); break;
    case R_X: value = scratchpad[R_I]; break;
    case R_Z: value = 0; break;
    default:  value = registers[reg];
  }
  return value;
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
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+1]) + 1);
  iptr += i_lengths[I];
}

void inc_scr() {
  reg_write(R_X, reg_read(R_X) + 1);
  iptr += i_lengths[IX];
}

void inc_idx() {
  scratchpad[instructions[iptr+1]] += 1;
  iptr += i_lengths[IJ];
}

void dec_reg() {
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+1]) - 1);
  iptr += i_lengths[D];
}

void dec_scr() {
  reg_write(R_X, reg_read(R_X) - 1);
  iptr += i_lengths[DX];
}

void dec_idx() {
  scratchpad[instructions[iptr+1]] -= 1;
  iptr += i_lengths[DJ];
}

void add_reg() {
  byte basis = reg_read(instructions[iptr+1]);
  reg_write(instructions[iptr+1], basis + reg_read(instructions[iptr+2]));
  iptr += i_lengths[A];
}

void add_lit() {
  byte basis = reg_read(instructions[iptr+1]);
  reg_write(instructions[iptr+1], basis + instructions[iptr+2]);
  iptr += i_lengths[AL];
}

void add_scr() {
  byte basis = reg_read(instructions[iptr+1]);
  reg_write(instructions[iptr+1], basis + reg_read(R_X));
  iptr += i_lengths[AX];
}

void add_idx() {
  byte basis = reg_read(instructions[iptr+1]);
  reg_write(instructions[iptr+1], basis + scratchpad[instructions[iptr+2]]);
  iptr += i_lengths[AJ];
}

void mult_reg() {
  word product = reg_read(instructions[iptr+1]) * reg_read(instructions[iptr+2]);
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  iptr += i_lengths[M];
}

void mult_lit() {
  word product = reg_read(instructions[iptr+1]) * instructions[iptr+2];
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  iptr += i_lengths[ML];
}

void mult_scr() {
  word product = reg_read(instructions[iptr+1]) * reg_read(R_X);
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  iptr += i_lengths[MX];
}

void mult_idx() {
  word product = reg_read(instructions[iptr+1]) * scratchpad[instructions[iptr+2]];
  reg_write(R_P, lowByte(product));
  reg_write(R_Q, highByte(product));
  iptr += i_lengths[MJ];
}

void load_reg() {
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+2]));
  iptr += i_lengths[L];
}

void load_lit() {
  reg_write(instructions[iptr+1], instructions[iptr+2]);
  iptr += i_lengths[LL];
}

void load_scr() {
  reg_write(instructions[iptr+1], reg_read(R_X));
  iptr += i_lengths[LX];
}

void load_idx() {
  reg_write(instructions[iptr+1], scratchpad[instructions[iptr+2]]);
  iptr += i_lengths[LJ];
}

/*
** Instruction implementations (loop)
*/

void skip_while() {
  byte nest_ct = 0;
  do {
    switch (instructions[iptr]) {
      case W: nest_ct++; break;
      case WL: nest_ct++; break;
      case WX: nest_ct++; break;
      case WJ: nest_ct++; break;
      case E: nest_ct--; break;
    }
    iptr += i_lengths[instructions[iptr]];
  } while (nest_ct > 0);
}

void while_reg() {
  if (reg_read(instructions[iptr+1]) == reg_read(instructions[iptr+2])) {
    skip_while();
  }
  else {
    push_ip();
    iptr += i_lengths[W];
  }
}

void while_lit() {
  if (reg_read(instructions[iptr+1]) == instructions[iptr+2]) {
    skip_while();
  }
  else {
    push_ip();
    iptr += i_lengths[WL];
  }
}

void while_scr() {
  if (reg_read(instructions[iptr+1]) == reg_read(R_X)) {
    skip_while();
  }
  else {
    push_ip();
    iptr += i_lengths[WX];
  }
}

void while_idx() {
  if (reg_read(instructions[iptr+1]) == scratchpad[instructions[iptr+2]]) {
    skip_while();
  }
  else {
    push_ip();
    iptr += i_lengths[WJ];
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
    reg_read(instructions[iptr+1]),
    reg_read(instructions[iptr+2]),
    reg_read(instructions[iptr+3])
  );
  iptr += i_lengths[P];
}

void pixel_lit() {
  leds[reg_read(R_L)].setRGB(
    instructions[iptr+1],
    instructions[iptr+2],
    instructions[iptr+3]
  );
  iptr += i_lengths[PL];
}

void pixel_idx() {
  leds[reg_read(R_L)].setRGB(
    scratchpad[instructions[iptr+1]],
    scratchpad[instructions[iptr+2]],
    scratchpad[instructions[iptr+3]]
  );
  iptr += i_lengths[PJ];
}

void pixel_add_reg() {
  // red_delta = reg_read(instructions[iptr+1]);
  // green_delta = reg_read(instructions[iptr+2]);
  // blue_delta = reg_read(instructions[iptr+3]);
  iptr += i_lengths[Q];
}

void pixel_add_lit() {
  // red_delta = instructions[iptr+1];
  // green_delta = instructions[iptr+2];
  // blue_delta = instructions[iptr+3];
  iptr += i_lengths[QL];
}

void pixel_add_idx() {
  // red_delta = scratchpad[instructions[iptr+1]];
  // green_delta = scratchpad[instructions[iptr+2]];
  // blue_delta = scratchpad[instructions[iptr+3]];
  iptr += i_lengths[QJ];
}

/*
** Instruction implementations (red)
*/

void pixel_red_reg() {
  leds[reg_read(R_L)].r = reg_read(instructions[iptr+1]);
  iptr += i_lengths[R];
}

void pixel_red_lit() {
  leds[reg_read(R_L)].r = instructions[iptr+1];
  iptr += i_lengths[RL];
}

void pixel_red_scr() {
  leds[reg_read(R_L)].r = reg_read(R_X);
  iptr += i_lengths[RX];
}

void pixel_red_idx() {
  leds[reg_read(R_L)].r = scratchpad[instructions[iptr+1]];
  iptr += i_lengths[RJ];
}

/*
** Instruction implementations (green)
*/

void pixel_green_reg() {
  leds[reg_read(R_L)].g = reg_read(instructions[iptr+1]);
  iptr += i_lengths[G];
}

void pixel_green_lit() {
  leds[reg_read(R_L)].g = instructions[iptr+1];
  iptr += i_lengths[GL];
}

void pixel_green_scr() {
  leds[reg_read(R_L)].g = reg_read(R_X);
  iptr += i_lengths[GX];
}

void pixel_green_idx() {
  leds[reg_read(R_L)].g = scratchpad[instructions[iptr+1]];
  iptr += i_lengths[GJ];
}

/*
** Instruction implementations (blue)
*/

void pixel_blue_reg() {
  leds[reg_read(R_L)].b = reg_read(instructions[iptr+1]);
  iptr += i_lengths[B];
}

void pixel_blue_lit() {
  leds[reg_read(R_L)].b = instructions[iptr+1];
  iptr += i_lengths[BL];
}

void pixel_blue_scr() {
  leds[reg_read(R_L)].b = reg_read(R_X);
  iptr += i_lengths[BX];
}

void pixel_blue_idx() {
  leds[reg_read(R_L)].b = scratchpad[instructions[iptr+1]];
  iptr += i_lengths[BJ];
}
