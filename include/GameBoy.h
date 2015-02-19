// DMGe - Gameboy emulator and debugger
// -------------------------------------
// GameBoy header. Defines the core class
// for the GameBoy.

#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <iostream>

class c_DMGCPU;
class c_GPU;
class c_MMU;
class c_Canvas;

class c_GameBoy {
    private:
        c_DMGCPU* CPU;
        c_MMU* MMU;
        c_GPU* GPU;
        c_Canvas* canvas;

    public:
        c_GameBoy(const char* romfname, c_Canvas* cnv);
        ~c_GameBoy();
        bool pause = true;

        void Run(void);
};

#endif // GAMEBOY_H
