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
boost::thread* dbgthread;

c_GameBoy* GameBoy;
c_Debugger* Debugger;

void InitialiseThreads(const char* romfname)
{
    GameBoy = new c_GameBoy(romfname);
    // Pass a pointer to the GameBoy class to the debugger. It will need this
    // for almost everything that it does.
    Debugger = new c_Debugger(GameBoy);

    // Create and start threads.
    emuthread = new boost::thread(boost::bind(&c_GameBoy::Run, GameBoy));
    emuthread->join();

    dbgthread = new boost::thread(boost::bind(&c_Debugger::Run, Debugger));
    dbgthread->join();
}

int main(int argc, char* argv[])
{
    std::cout << "DMGe" << std::endl;

    InitialiseThreads("test.gb");

    return 0;
}
