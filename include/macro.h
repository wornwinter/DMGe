#ifndef MACRO_H
#define MACRO_H

#define BytesToWord(A, B) ((B << 8) | A)
#define UpperByte(A) ((A & 0xFF00) >> 8)
#define LowerByte(A) (A & 0x00FF)

#endif // MACRO_H
