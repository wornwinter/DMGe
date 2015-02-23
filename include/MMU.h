#ifndef MMU_H
#define MMU_H

#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "debug.h"
#include "macro.h"

#define SET_INT_VBLANK(x)   intflags = ((intflags & 0xFE) | x) //Set bit 0
#define SET_INT_LCDSTAT(x)  intflags = ((intflags & 0xFD) | (x << 1)) //Set bit 1
#define SET_INT_TIMER(x)    intflags = ((intflags & 0xFB) | (x << 2))
#define SET_INT_SERIAL(x)   intflags = ((intflags & 0xF7) | (x << 3))
#define SET_INT_JOYPAD(x)   intflags = ((intflags & 0xEF) | (x << 4))

class c_GPU;

class c_MMU {
    private:
        uint8_t memory[0xFFFF];
        uint8_t bios[0xFF1];
        uint8_t rom[0xFF][0x4000]; //16k ROM banks.
        uint8_t vram[0x2000]; //8k Video RAM.
        uint8_t eram[0x2000]; //8k External RAM.
        uint8_t wram[0x2000]; //8k Working RAM.
        uint8_t oam[0xA0];
        uint8_t zram[0x7F];


        uint8_t activerombank = 1;
        bool biosmapped = false;
        c_GPU* GPU;

    public:
        uint8_t intflags = 0x00; //Interrupt flags register.
        uint8_t intenable = 0x00;

        c_MMU(c_GPU* pGPU);
        ~c_MMU();
        uint8_t ReadByte(uint16_t addr);
        uint16_t ReadWord(uint16_t addr);

        void WriteByte(uint16_t addr, uint8_t data);
        void WriteWord(uint16_t addr, uint16_t data);

        void MapBIOS(bool ismap);
        void LoadBIOS(const char *fname);
        void LoadROM(const char *fname);


};

#endif // MMU_H
