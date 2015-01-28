#ifndef DMGCPU_H
#define DMGCPU_H

#include <stdint.h>

typedef union
{
    struct
    {
        uint8_t hi;
        uint8_t lo;
    };
    uint16_t word;
}Register16_t;

//Our CPU regisgers
typedef struct
{
    //Our basic register, AF, BC, DE, HL
    Register16_t AF, BC, DE, HL;

    //Special Registers
    Register16_t SP, PC;
}regs_t;

class c_DMGCPU
{
public:
    c_DMGCPU();
    ~c_DMGCPU();

private:
    regs_t Registers;

};



#endif // DMGCPU_H_INCLUDED
