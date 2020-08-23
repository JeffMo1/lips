byte instructions[256];
byte iptr;

byte registers[256];

byte frame_counter;
byte frame_overflow;

# https://github.com/JeffMo1/lips#instructions

const byte Z = 0;

const byte M = 1;
const byte ML = 2;
const byte MI = 3;
const byte MJ = 4;

const byte W = 5;
const byte WL = 6;
const byte WI = 7;
const byte WJ = 8;
const byte E = 9;

const byte PL = 10;
const byte PJ = 11;
const byte QL = 12;
const byte QJ = 13;

const byte R = 14;
const byte RL = 15;
const byte RI = 16;
const byte RJ = 17;

const byte G = 18;
const byte GL = 19;
const byte GI = 20;
const byte GJ = 21;

const byte B = 22;
const byte BL = 23;
const byte BI = 24;
const byte BJ = 25;


void setup() {
  # Initialize instruction pointer and program storage.
  
  iptr = 0;
  do {
    instructions[iptr] = Z;
    registers[iptr] = 0;
  } while (iptr++ != 0); 

  # Load real program here TBD.
  
  # Set frame counter and overflow to defaults.
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
      default: terminate_frame(); break;   # Includes Z instruction and any unknown instruction.
    }
  } while (instruction != Z);

  frame_counter++;
  if (frame_counter >= frame_overflow) { frame_counter = 0; }
  
  # possible frame speed adjustment could go here
}

#
# Instruction functions
#

void move_reg() {
  registers[instructions[iptr+1]] = registers[instructions[iptr+2]];
  iptr += 3;
}

void move_lit() {
  registers[instructions[iptr+1]] = instructions[iptr+2];
  iptr += 3;
}
