#include "GameBoy.h"

c_GameBoy::c_GameBoy(const char* romfname)
{
    MMU = new c_MMU();
    CPU = new c_DMGCPU(MMU);
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
    CPU->Tick();
}
