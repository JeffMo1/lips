// LED definitions

#include <FastLED.h>
#define NUM_LEDS 150
#define DATA_PIN 12

CRGB leds[NUM_LEDS];

// https://github.com/JeffMo1/lips/#program-structure

byte iptr;
byte yptr;

byte stack[16];
byte sptr;

// https://github.com/JeffMo1/lips/#registers

byte registers[256];

const byte R_N = 255;
const byte R_S = 254;
const byte R_R = 253;
const byte R_U = 252;
const byte R_V = 251;
const byte R_P = 250;
const byte R_Q = 249;
const byte R_Z = 248;

const byte R_L = 247;

const byte R_F = 246;
const byte R_O = 245;

const byte R_I = 244;
const byte R_X = 243;

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

const byte L = 19;
const byte LL = 20;
const byte LX = 21;
const byte LJ = 22;

const byte W = 23;
const byte WL = 24;
const byte WX = 25;
const byte WJ = 26;
const byte E = 27;

const byte P = 28;
const byte PL = 29;
const byte PJ = 30;

const byte Q = 31;
const byte QL = 32;
const byte QJ = 33;

const byte R = 34;
const byte RL = 35;
const byte RX = 36;
const byte RJ = 37;

const byte G = 38;
const byte GL = 39;
const byte GX = 40;
const byte GJ = 41;

const byte B = 42;
const byte BL = 43;
const byte BX = 44;
const byte BJ = 45;

const byte Y = 46;

const byte i_lengths[] = {
  1,                    // Z
  2, 1, 2,              // IC
  2, 1, 2,              // DC
  3, 3, 2, 3,           // A
  3, 3, 2, 3,           // M
  3, 3, 2, 3,           // DV
  3, 3, 2, 3,           // L
  3, 3, 2, 3, 1,        // W
  4, 4, 4,              // P
  4, 4, 4,              // Q
  3, 3, 2, 3,           // R
  3, 3, 2, 3,           // G
  3, 3, 2, 3,           // B
  1,                    // Y
};


byte instructions[256] = {
    LL, R_V, 8,
    L, 0, R_R,
    L, 1, R_R,
    L, 2, R_R,
    Y,
    LL, R_L, 0,
    W, R_L, R_N,
      L, 10, 0,
      A, 10, R_F,
      ML, 10, 10,
      L, 10, R_P,
      L, 11, 1,
      A, 11, R_F,
      ML, 11, 10,
      L, 11, R_P,
      L, 12, 2,
      A, 12, R_F,
      ML, 12, 10,
      L, 12, R_P,
      P, 10, 11, 12,
      IC, R_L,
    E,
    Z};

void setup() {
  // Initialize FastLED

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Initialize IP, SP, and program storage.
  
  iptr = 0;
  yptr = 0;
  sptr = 0;
  do {
    registers[iptr] = 0;
    scratchpad[iptr] = 0;
  } while (iptr++ != 0); 
   
  init_system();
  init_frame();
  
  delay(1000);
}

void ins(byte data, int ms) {
  for (int x=0; x<8; x++) {
    leds[x] = bitRead(data, x) ? CRGB::Red : CRGB::Black;
  }
  FastLED.show();
  delay(ms);
  for (int x=0; x<8; x++) { leds[x] = CRGB::Black; }
  FastLED.show();
}

void param(byte data, int ms) {
  for (int x=0; x<8; x++) {
    leds[x] = bitRead(data, x) ? CRGB::Green : CRGB::Black;
  }
  FastLED.show();
  delay(ms);
  for (int x=0; x<8; x++) { leds[x] = CRGB::Black; }
  FastLED.show();  
}

void loop() {
  byte instruction;
  
  iptr = yptr;
  sptr = 0 ;
  do {
    instruction = instructions[iptr];
    // debug: ins(instruction, 500);
    switch (instruction) {
      case IC:  inc_reg(); break;
      case ICX: inc_scr(); break;
      case ICJ: inc_idx(); break;
      case DC:  dec_reg(); break;
      case DCX: dec_scr(); break;
      case DCJ: dec_idx(); break;
      case A:  add_reg(); break;
      case AL: add_lit(); break;
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
      case Y:  set_yptr(); break;
      default: terminate_frame(); break;   // Includes Z instruction and any unknown instruction.
    }
  } while (instruction != Z);

  advance_frame();
  delay(5); // possible frame speed adjustment could go here
}

/*
** Register management 
*/

void init_frame() {
  registers[R_F] = 0;
  registers[R_O] = 16;
}

void advance_frame() {
  registers[R_F]++;
  check_frame_overflow();
}

void check_frame_overflow() {
  if (registers[R_F] >= registers[R_O]) { registers[R_F] = 0; }
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

void div_reg() {
  byte basis = reg_read(instructions[iptr+1]);
  byte divisor = reg_read(instructions[iptr+2]);
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
}

void div_lit() {
  byte basis = reg_read(instructions[iptr+1]);
  byte divisor = instructions[iptr+1];
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
}

void div_scr() {
  byte basis = reg_read(instructions[iptr+1]);
  byte divisor = reg_read(R_X);
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
}

void div_idx() {
  byte basis = reg_read(instructions[iptr+1]);
  byte divisor = scratchpad[instructions[iptr+2]];
  reg_write(R_P, basis % divisor);
  reg_write(R_Q, basis / divisor);
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

/*
** Instruction implementations (miscellaneous)
*/

void set_yptr() {
  yptr = iptr + i_lengths[Y];
  iptr = yptr;
}
