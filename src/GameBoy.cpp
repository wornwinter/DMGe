#include "GameBoy.h"

c_GameBoy::c_GameBoy(const char* romfname)
{
    MMU = new c_MMU();
    CPU = new c_DMGCPU(MMU);
    MMU->LoadBIOS("roms/bios.bin");
    //At the minute this should be a rom without bank switching. Tetris for example.
    MMU->LoadROM(romfname);
}

c_GameBoy::~c_GameBoy()
{
}

void c_GameBoy::Run(void)
{
    std::cout << "GameBoy thread running." << std::endl;
    while(1)
    {
        CPU->Tick();
    }
}
