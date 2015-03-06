#ifndef INPUT_H
#define INPUT_H

#include <cstdint>
#include <wx/event.h>
#include "MMU.h"

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

    EDIT: A better way to do this is as follows:
    First, check whether we are working with the D-pad or the "action" buttons (A, B, STRT, SLCT).
    P14 (BIT 4) = We are performing an action with the D-Pad
    P15 (BIT 5) = We are performing an action with the action buttons.

    NOTE: Thanks to the *awesome* engineers at Nintendo, (8 buttons, not using all 8-bits for which button is selected...)
    when something is selected, 0 means ON and a 1 means OFF...

    TODO: How do we store these keys?
**/

//Ports..
#define P15_SELECT  0x10
#define P14_DPAD    0x20

//Buttons
#define P13         0x08
#define P13         0x04
#define P13         0x02
#define P13         0x01

class c_Input
{
    private:
        uint8_t joypad;

    public:
        c_Input();
        ~c_Input();
        void Tick(c_MMU* MMU);
        void WriteReg(uint8_t address, uint8_t data);
        void ReadReg(uint8_t address);
};


#endif // INPUT_H
