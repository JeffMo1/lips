byte program[256];
byte iptr;

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
    program[iptr++] = I_Z;
  } while (iptr != 0); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
