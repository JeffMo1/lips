#ifndef LIPS_INSTRUCTIONS_H
#define LIPS_INSTRUCTIONS_H

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

#endif
