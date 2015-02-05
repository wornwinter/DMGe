#include "GameBoy.h"

c_GameBoy::c_GameBoy(const char* romfname)
{
    MMU = new c_MMU();
    CPU = new c_DMGCPU(MMU);
}

c_GameBoy::~c_GameBoy()
{
}

void c_GameBoy::Run(void)
{
    std::cout << "GameBoy thread running." << std::endl;
}
