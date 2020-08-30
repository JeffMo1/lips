byte instructions[256];
byte iptr;

byte registers[256];

byte frame_counter;
byte frame_overflow;

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
  } while (iptr++ != 0); 

  // Load real program here TBD.
  
  // Set frame counter and overflow to defaults.
  frame_counter = 0;
  frame_overflow = 10;
}

void loop() {
  byte instruction;
  
  iptr = 0;
  do {
    instruction = instructions[iptr];
    switch (instruction) {
      case M:  move_reg(); break;
      case ML: move_lit(); break;
      case MI: move_scr(); break;
      case MJ: move_idx(); break;
      case W:  while_reg(); break;
      case WL: while_lit(); break;
      case WI: while_scr(); break;
      case WJ: while_idx(); break;
      case E:  end_while(); break;
      case PL: pixel_lit(); break;
      case PJ: pixel_idx(); break;
      case QL: pixel_add_lit(); break;
      case QJ: pixel_add_idx(); break;
      case R:  pixel_red_reg(); break;
      case RL: pixel_red_lit(); break;
      case RI: pixel_red_scr(); break;
      case RJ: pixel_red_idx(); break;
      case G:  pixel_green_reg(); break;
      case GL: pixel_green_lit(); break;
      case GI: pixel_green_scr(); break;
      case GJ: pixel_green_idx(); break;
      case B:  pixel_blue_reg(); break;
      case BL: pixel_blue_lit(); break;
      case BI: pixel_blue_scr(); break;
      case BJ: pixel_blue_idx(); break;
      default: terminate_frame(); break;   // Includes Z instruction and any unknown instruction.
    }
  } while (instruction != Z);

  frame_counter++;
  if (frame_counter >= frame_overflow) { frame_counter = 0; }
  
  // possible frame speed adjustment could go here
}

/*
** Instruction functions
*/

void move_reg() {
  registers[instructions[iptr+1]] = registers[instructions[iptr+2]];
  iptr += 3;
}

void move_lit() {
  registers[instructions[iptr+1]] = instructions[iptr+2];
  iptr += 3;
}
