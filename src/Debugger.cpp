#include "Debugger.h"

c_Debugger::c_Debugger(c_GameBoy *GB)
{
    GameBoy = GB;
}

c_Debugger::~c_Debugger()
{
}

void c_Debugger::Run(void)
{
    std::cout << "Debugger thread running." << std::endl;
}
