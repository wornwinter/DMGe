// DMGe - Gameboy emulator and debugger
// -------------------------------------
// GameBoy header. Defines the core class
// for the GameBoy.

#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <iostream>

class c_GameBoy {
    public:
        c_GameBoy();
        ~c_GameBoy();

        void Run(void);
};

#endif // GAMEBOY_H
