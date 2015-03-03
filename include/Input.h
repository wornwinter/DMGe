#ifndef INPUT_H
#define INPUT_H

#include <cstdint>

/**
    Okay, this is a kind of complicated process, but I think I get it.
    The way the input is physically wired is in a 2x4 matrix, like so...
                  P14        P15
                  |          |
        P10-------O-Right----O-A
                  |          |
        P11-------O-Left-----O-B
                  |          |
        P12-------O-Up-------O-Select
                  |          |
        P13-------O-Down-----O-Start
                  |          |

    How I think we can detect keys, is reading which ports are on in pairs.
    For example, when reading if A is being pushed, we check for P10 AND P15.
    If P15 is on, as well as P10, we know an A key signal MUST be being sent.

    Oh, also, we need to check for an interrupt somewhere. Maybe we should have
    an Request Interrupt (IRQ) function

    TODO: How do we store these keys?
**/

class c_Input
{
    private:
        c_MMU* MMU;
        uint8_t joypad;

    public:
        c_Input();
        ~c_Input();
        void Tick(c_MMU* MMU);
        void WriteReg(uint8_t address, uint8_t data);
        void ReadReg(uint8_t address);
};


#endif // INPUT_H
