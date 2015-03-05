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

void c_Input::ReadReg(uint8_t address)
{

}


