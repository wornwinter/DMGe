#include "Debugger.h"
#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>



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
    MainLoop();
}

void c_Debugger::MainLoop(void)
{
    std::string command;

    while(run)
    {
        std::cout << "gbdb > ";
        std::getline(std::cin, command);
        std::cout << std::endl;

        if(command.length() > 0)
        {
            // Was a command actually entered? If so, lets parse it.
        }

        command.clear(); // Clear string for next command.
    }
}
