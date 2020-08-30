// https://github.com/JeffMo1/lips/#program-structure

byte instructions[256];
byte iptr;


// https://github.com/JeffMo1/lips/#registers

byte registers[256];

const byte R_N = 255;
const byte R_S = 254;
const byte R_R = 253;
const byte R_P = 252;
const byte R_Q = 251;

const byte R_L = 250;

const byte R_F = 249;
const byte R_V = 248;

const byte R_I = 247;
const byte R_X = 246;

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

const byte PL = 24;
const byte PJ = 25;
const byte QL = 26;
const byte QJ = 27;

const byte R = 28;
const byte RL = 29;
const byte RX = 30;
const byte RJ = 31;

const byte G = 32;
const byte GL = 33;
const byte GX = 34;
const byte GJ = 35;

const byte B = 36;
const byte BL = 37;
const byte BX = 38;
const byte BJ = 39;

void setup() {
  // Initialize instruction pointer and program storage.
  
  iptr = 0;
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
  do {
    instruction = instructions[iptr];
    switch (instruction) {
      case I:  inc_reg(); break;
      case IX: inc_scr(); break;
      case IJ: inc_idx(); break;
      case D:  dec_reg(); break;
      case DX: dec_scr(); break;
      case DJ: dec_idx(); break;
      case L:  load_reg(); break;
      case LL: load_lit(); break;
      case LX: load_scr(); break;
      case LJ: load_idx(); break;
      case W:  while_reg(); break;
      case WL: while_lit(); break;
      case WX: while_scr(); break;
      case WJ: while_idx(); break;
      case E:  end_while(); break;
      case PL: pixel_lit(); break;
      case PJ: pixel_idx(); break;
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
  
  // possible frame speed adjustment could go here
}

/*
** Register management 
*/

void init_frame() {
  registers[R_F] = 0;
  registers[R_V] = 10;
}

void advance_frame() {
  registers[R_F]++;
  check_frame_overflow();
}

void check_frame_overflow() {
  if (registers[R_F] >= registers[R_V]) { registers[R_V] = 0; }
}

void init_system() {
  registers[R_N] = 150;  // TBD: Should use the LED const for the library
  registers[R_S] = 0;    // TBD: Read this from hardware pins
  registers[R_R] = random(256);
  registers[R_P] = 0;
  registers[R_Q] = 0;
}

byte reg_read(byte reg) {
  byte value;

  switch (reg) {
    case R_R: value = registers[R_R]; registers[R_R] = random(256); break;
    case R_X: value = scratchpad[R_I]; break;
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
    case R_X: scratchpad[R_I] = data; break;
    default:  registers[reg] = data;
  }
}
/*
** Instruction implementations
*/

void inc_reg() {
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+1] + 1));
  iptr += 2;
}

void inc_scr() {
  reg_write(R_X, reg_read(R_X) + 1);
  iptr += 1;
}

void inc_idx() {
  scratchpad[instructions[iptr+1]] += 1;
  iptr += 2;
}

void dec_reg() {
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+1] - 1));
  iptr += 2;
}

void dec_scr() {
  reg_write(R_X, reg_read(R_X) - 1);
  iptr += 1;
}

void dec_idx() {
  scratchpad[instructions[iptr+1]] -= 1;
  iptr += 2;
}

void load_reg() {
  reg_write(instructions[iptr+1], reg_read(instructions[iptr+2]));
  iptr += 3;
}

void load_lit() {
  reg_write(instructions[iptr+1], instructions[iptr+2]);
  iptr += 3;
}

void load_scr() {
  reg_write(instructions[iptr+1], reg_read(R_X));
  iptr += 2;
}

void load_idx() {
  reg_write(instructions[iptr+1], scratchpad[instructions[iptr+2]]);
  iptr += 3;
}
