// DMGe - Gameboy emulator and debugger
// -------------------------------------
// GameBoy header. Defines the core class
// for the GameBoy.

#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <iostream>
#include "DMGCPU.h"
#include "MMU.h"

class c_GameBoy {
    private:
        c_DMGCPU* CPU;
        c_MMU* MMU;

    public:
        c_GameBoy(const char* romfname);
        ~c_GameBoy();
        bool pause = true;

        void Run(void);
};

#endif // GAMEBOY_H
