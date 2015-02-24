#include "GameBoy.h"
#include "DMGCPU.h"
#include "GPU.h"
#include "MMU.h"
#include "canvas.h"
#include <boost/thread/thread.hpp> //For threading. wxThreads aren't great.
#include <boost/date_time.hpp>

c_GameBoy::c_GameBoy(const char* romfname, c_Canvas* cnv)
{
    canvas = cnv;
    GPU = new c_GPU(canvas);
    MMU = new c_MMU(GPU);
    CPU = new c_DMGCPU(MMU);
    cart = new Cartridge();

    MMU->LoadBIOS("roms/bios.bin");
    //At the minute this should be a rom without bank switching. Tetris for example.
    MMU->LoadROM(romfname);
    MMU->MapBIOS(true);
}

c_GameBoy::~c_GameBoy()
{
}

//Called by application in event loop.
void c_GameBoy::Run(void)
{
    boost::posix_time::microseconds worktime(50);
    while(1)
    {
        if(!pause)
        {
           CPU->Tick();
           GPU->Tick(CPU->GetClock(), MMU);
        }
        boost::this_thread::sleep(worktime);
    }
}
