// DMGe - Gameboy emulator and debugger
// -------------------------------------


#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include "GameBoy.h"

class c_Debugger {
    private:
        c_GameBoy *GameBoy;

    public:
        c_Debugger(c_GameBoy *GB);
        ~c_Debugger();

        void Run(void);
};

#endif // DEBUGGER_H
