#include "Debugger.h"
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
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
    std::string* commandtokens;


    while(run)
    {
        std::cout << "gbdb > ";
        std::getline(std::cin, command);

        if(command.length() > 0)
        {
            // Was a command actually entered? If so, lets parse it.

            // Start by separating input into tokens. Can be separated by commas or spaces.
            boost::tokenizer<> token(command);
            std::vector<std::string> v(token.begin(), token.end());
            commandtokens = &v[0];

            if(commandtokens[0] == "run")
            {
                if(v.size() == 1)
                {
                    // Right number of arguments. Run.
                }
                else {
                    std::cout << "Command does not require arguments.";
                }
            }
            else if(commandtokens[0] == "break")
            {
                if(v.size() == 2)
                {
                    // We've got the right number of arguments. Set breakpoint.
                }
                else {
                    // Wrong number of arguments. Show help message.
                    std::cout << "Invalid number of arguments." << std::endl << "Usage: 'break [address]'";
                }
            }
            else if(commandtokens[0] == "read")
            {
            }
            else if(commandtokens[0] == "write")
            {
            }
            else if(commandtokens[0] == "regs")
            {
            }
            else {
                std::cout << "Error, unknown command: " << commandtokens[0];
            }

            std::cout << std::endl;

        }

        command.clear(); // Clear string for next command.
    }
}
