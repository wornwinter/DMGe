#ifndef MACRO_H
#define MACRO_H

#define BytesToWord(A, B) ((A << 8) | B)
#define UpperByte(A) ((A & 0xFF00) >> 8)
#define LowerByte(A) (A & 0x00FF)

#endif // MACRO_H
