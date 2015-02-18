#ifndef GPU_H
#define GPU_H

#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
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

    public:
        c_GPU();
        ~c_GPU();
        void Tick(uint32_t clock);

};
#endif // GPU_H
