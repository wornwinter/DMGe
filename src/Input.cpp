#include "Input.h"

c_Input::c_Input()
{
    joypad = 0xFF; //Set joypad to all on
}

c_Input::~c_Input()
{

}

void c_Input::Tick(c_MMU* MMU)
{
    uint8_t joyio = MMU->ReadByte(0xFF00); //Read te joypad control reg

    //We must have pushed some keys for this to be different!
    if(joyio != joypad)
    {
        MMU->WriteByte(0xFF00, joypad);
        MMU->intflags |= 8; //Joypad interrupt!
    }
}

void c_Input::WriteReg(uint8_t address, uint8_t data)
{

}

//We probably dont need address here..
void c_Input::ReadReg(uint8_t address)
{
    uint8_t joyio = MMU->ReadByte(0xFF00);

    //Goddamnit, Nintendo...
    //Check for directional buttons, bit OFF!!
    if(!(joyio & P14_DPAD)
    {
        uint8_t dpad = joyio & 0x0F; //Mask last 4-bits

        //Down was pushed
        if(!(dpad & P13)){}
        else if(!(dpad & P12)){} //Up was pushed!
        else if(!(dpad & P11)){} //Left was pushed
        else if(!(dpad & P10)){} //Right was pushed
    }
}


