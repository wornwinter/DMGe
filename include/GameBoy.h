// DMGe - Gameboy emulator and debugger
// -------------------------------------
// GameBoy header. Defines the core class
// for the GameBoy.

#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <iostream>
#include "DMGCPU.h"

class c_GameBoy {
    private:
        c_DMGCPU* CPU;

    public:
        c_GameBoy();
        ~c_GameBoy();

        void Run(void);
};

#endif // GAMEBOY_H
