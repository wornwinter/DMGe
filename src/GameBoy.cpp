#include "GameBoy.h"

c_GameBoy::c_GameBoy()
{
    CPU = new c_DMGCPU();
}

c_GameBoy::~c_GameBoy()
{
}

void c_GameBoy::Run(void)
{
    std::cout << "GameBoy thread running." << std::endl;
}
