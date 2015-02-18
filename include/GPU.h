#ifndef GPU_H
#define GPU_H

#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "debug.h"
#include "macro.h"

//GPU state definitions.
#define STATE_OAM_READ 2
#define STATE_VRAM_READ 3
#define STATE_HBLANK 0
#define STATE_VBLANK 1

class c_GPU {

    private:
        uint32_t stateclock;
        uint8_t state;
        uint8_t line;
        uint8_t tileset[384][8];
        uint8_t vram[0x1FFF];

    public:
        c_GPU();
        ~c_GPU();
        void WriteByte(uint16_t addr, uint8_t data);
        void UpdateTile(uint16_t addr, uint8_t data);

        void Tick(uint32_t clock);

};
#endif // GPU_H
