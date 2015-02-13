// DMGe - Gameboy emulator and debugger
// -------------------------------------
// Main program file. Execution begins
// here.

#include <boost/thread/thread.hpp>
#include <iostream>
#include <string>
#include "GameBoy.h"
#include "Debugger.h"

// The emulator and the debugger will run on separate threads.
// This allows them to operate independantly of eachother.
boost::thread* emuthread;

c_GameBoy* GameBoy;

void InitialiseThreads(const char* romfname)
{
    GameBoy = new c_GameBoy(romfname);

    // Create and start threads.

    emuthread = new boost::thread(boost::bind(&c_GameBoy::Run, GameBoy));
    emuthread->join();
}

int main(int argc, char* argv[])
{
    std::cout << "DMGe" << std::endl;

    InitialiseThreads("roms/tetris.gb");

    return 0;
}
