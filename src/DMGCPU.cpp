#include "DMGCPU.h"
#include "DMG_opcodes.h"
#include "debug.h"
#include <cstring>

c_DMGCPU::c_DMGCPU(c_MMU* pMMU)
{
    InitOpcodeTables();
    MMU = pMMU;

    //Reset everything to zero. Stops a weird bug where the PC starts at the top of GB memory.
    memset(&Registers, 0, sizeof(Registers));

#ifdef _DEBUG
    //Use this for testing roms without the BIOS.
    //Registers.PC.word = 0x0100; //Skips that huge loop at the start for debugging purposes.
    //Registers.SP.word = 0xFFFE;
#endif // _DEBUG

    logfile.open("trace.txt", std::ios_base::app | std::ios_base::out);
}

c_DMGCPU::~c_DMGCPU()
{

}

//Returns clock periods taken for last instruction.
uint32_t c_DMGCPU::GetClock()
{
    return Clock.t;
}

//Run one instruction.
void c_DMGCPU::Tick()
{
    if(Registers.PC.word == 0x0100)
        printinst = true;

    if(printinst && writelog)
    {
    logfile << "[" << std::hex << std::setw(4) << std::setfill('0') << Registers.PC.word << "] - "
            << DMG_opcodes[MMU->ReadByte(Registers.PC.word)] << "         - ";
    logfile << std::hex << "AF: " << Registers.AF.word << " BC: " << Registers.BC.word << " DE: " << Registers.DE.word
            << " HL: " << Registers.HL.word << " SP: " << Registers.SP.word << std::endl;
    }

    if(IME && running)
    {
        //Do interrupts.
        //Check interrupt that has fired. Mask off disabled interrupts.
        uint8_t intfired = MMU->intenable & MMU->intflags;
        if(intfired & 0x1)
        {
            DbgOut(DBG_CPU, VERBOSE_0, "Interrupt: Calling VBLANK service routine.");
            logfile << "Vblank!" << std::endl;
            //Call interrupt service routine.
            Registers.SP.word -= 2;
            MMU->WriteWord(Registers.SP.word, Registers.PC.word);
            Registers.PC.word = 0x0040; //ISR is always at 0x0040 for vblank.
            IME = false;
        }
        else if(intfired & 0x4) //Serial IRQ
        {
            DbgOut(DBG_CPU, VERBOSE_2, "Interrupt: Calling SERIAL service routine.");
            Registers.SP.word -= 2;
            MMU->WriteWord(Registers.SP.word, Registers.PC.word);
            Registers.PC.word = 0x0058;
            IME = false;
        }
    }

    if(running)
    {
        switch(MMU->ReadByte(Registers.PC.word))
        {
            case 0xCB:
                //0xCB special opcode. Look up in separate opcode table.
                if((this->OPCodesCB[MMU->ReadByte(Registers.PC.word+1)]) == NULL)
                    IllegalOperation(MMU->ReadByte(Registers.PC.word), true);
                else {
                    DbgOut(DBG_CPU, VERBOSE_2, "0xCB opcode: 0x%x", MMU->ReadByte(Registers.PC.word + 1));
                    (this->*OPCodesCB[MMU->ReadByte(Registers.PC.word + 1)])();
                }

            break;

            default:
                if((this->OPCodes[MMU->ReadByte(Registers.PC.word)]) == NULL)
                    IllegalOperation(MMU->ReadByte(Registers.PC.word), false);
                else
                //Anything else. Hopefully a standard opcode.
                (this->*OPCodes[MMU->ReadByte(Registers.PC.word)])();
            break;
        }

        //Keep track of machine and clock cycles.
        ClockTotal.m += Clock.m;
        ClockTotal.t += Clock.t;
    }
}

void c_DMGCPU::InitOpcodeTables()
{
    //Fill our opcode table
    OPCodes[0x00] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x01] = &c_DMGCPU::OPCode0x01;
    OPCodes[0x02] = &c_DMGCPU::OPCode0x02;
    OPCodes[0x03] = &c_DMGCPU::OPCode0x03;
    OPCodes[0x04] = &c_DMGCPU::OPCode0x04;
    OPCodes[0x05] = &c_DMGCPU::OPCode0x05;
    OPCodes[0x06] = &c_DMGCPU::OPCode0x06;
    OPCodes[0x07] = &c_DMGCPU::OPCode0x07;
    OPCodes[0x08] = &c_DMGCPU::OPCode0x08;
    OPCodes[0x09] = &c_DMGCPU::OPCode0x09;
    OPCodes[0x0A] = &c_DMGCPU::OPCode0x0A;
    OPCodes[0x0B] = &c_DMGCPU::OPCode0x0B;
    OPCodes[0x0C] = &c_DMGCPU::OPCode0x0C;
    OPCodes[0x0D] = &c_DMGCPU::OPCode0x0D;
    OPCodes[0x0E] = &c_DMGCPU::OPCode0x0E;
    OPCodes[0x0F] = &c_DMGCPU::OPCode0x0F;
    OPCodes[0x10] = &c_DMGCPU::OPCode0x10;
    OPCodes[0x11] = &c_DMGCPU::OPCode0x11;
    OPCodes[0x12] = &c_DMGCPU::OPCode0x12;
    OPCodes[0x13] = &c_DMGCPU::OPCode0x13;
    OPCodes[0x14] = &c_DMGCPU::OPCode0x14;
    OPCodes[0x15] = &c_DMGCPU::OPCode0x15;
    OPCodes[0x16] = &c_DMGCPU::OPCode0x16;
    OPCodes[0x17] = &c_DMGCPU::OPCode0x17;
    OPCodes[0x18] = &c_DMGCPU::OPCode0x18;
    OPCodes[0x19] = &c_DMGCPU::OPCode0x19;
    OPCodes[0x1A] = &c_DMGCPU::OPCode0x1A;
    OPCodes[0x1B] = &c_DMGCPU::OPCode0x1B;
    OPCodes[0x1C] = &c_DMGCPU::OPCode0x1C;
    OPCodes[0x1D] = &c_DMGCPU::OPCode0x1D;
    OPCodes[0x1E] = &c_DMGCPU::OPCode0x1E;
    OPCodes[0x1F] = &c_DMGCPU::OPCode0x1F;
    OPCodes[0x20] = &c_DMGCPU::OPCode0x20;
    OPCodes[0x21] = &c_DMGCPU::OPCode0x21;
    OPCodes[0x22] = &c_DMGCPU::OPCode0x22;
    OPCodes[0x23] = &c_DMGCPU::OPCode0x23;
    OPCodes[0x24] = &c_DMGCPU::OPCode0x24;
    OPCodes[0x25] = &c_DMGCPU::OPCode0x25;
    OPCodes[0x26] = &c_DMGCPU::OPCode0x26;
    OPCodes[0x27] = &c_DMGCPU::OPCode0x27;
    OPCodes[0x28] = &c_DMGCPU::OPCode0x28;
    OPCodes[0x29] = &c_DMGCPU::OPCode0x29;
    OPCodes[0x2A] = &c_DMGCPU::OPCode0x2A;
    OPCodes[0x2B] = &c_DMGCPU::OPCode0x2B;
    OPCodes[0x2C] = &c_DMGCPU::OPCode0x2C;
    OPCodes[0x2D] = &c_DMGCPU::OPCode0x2D;
    OPCodes[0x2E] = &c_DMGCPU::OPCode0x2E;
    OPCodes[0x2F] = &c_DMGCPU::OPCode0x2F;
    OPCodes[0x30] = &c_DMGCPU::OPCode0x30;
    OPCodes[0x31] = &c_DMGCPU::OPCode0x31;
    OPCodes[0x32] = &c_DMGCPU::OPCode0x32;
    OPCodes[0x33] = &c_DMGCPU::OPCode0x33;
    OPCodes[0x34] = &c_DMGCPU::OPCode0x34;
    OPCodes[0x35] = &c_DMGCPU::OPCode0x35;
    OPCodes[0x36] = &c_DMGCPU::OPCode0x36;
    OPCodes[0x37] = &c_DMGCPU::OPCode0x37;
    OPCodes[0x38] = &c_DMGCPU::OPCode0x38;
    OPCodes[0x39] = &c_DMGCPU::OPCode0x39;
    OPCodes[0x3A] = &c_DMGCPU::OPCode0x3A;
    OPCodes[0x3B] = &c_DMGCPU::OPCode0x3B;
    OPCodes[0x3C] = &c_DMGCPU::OPCode0x3C;
    OPCodes[0x3D] = &c_DMGCPU::OPCode0x3D;
    OPCodes[0x3E] = &c_DMGCPU::OPCode0x3E;
    OPCodes[0x3F] = &c_DMGCPU::OPCode0x3F;
    OPCodes[0x40] = &c_DMGCPU::OPCode0x40;
    OPCodes[0x41] = &c_DMGCPU::OPCode0x41;
    OPCodes[0x42] = &c_DMGCPU::OPCode0x42;
    OPCodes[0x43] = &c_DMGCPU::OPCode0x43;
    OPCodes[0x44] = &c_DMGCPU::OPCode0x44;
    OPCodes[0x45] = &c_DMGCPU::OPCode0x45;
    OPCodes[0x46] = &c_DMGCPU::OPCode0x46;
    OPCodes[0x47] = &c_DMGCPU::OPCode0x47;
    OPCodes[0x48] = &c_DMGCPU::OPCode0x48;
    OPCodes[0x49] = &c_DMGCPU::OPCode0x49;
    OPCodes[0x4A] = &c_DMGCPU::OPCode0x4A;
    OPCodes[0x4B] = &c_DMGCPU::OPCode0x4B;
    OPCodes[0x4C] = &c_DMGCPU::OPCode0x4C;
    OPCodes[0x4D] = &c_DMGCPU::OPCode0x4D;
    OPCodes[0x4E] = &c_DMGCPU::OPCode0x3E;
    OPCodes[0x4F] = &c_DMGCPU::OPCode0x4F;
    OPCodes[0x50] = NULL;
    OPCodes[0x51] = NULL;
    OPCodes[0x52] = NULL;
    OPCodes[0x53] = NULL;
    OPCodes[0x54] = &c_DMGCPU::OPCode0x54;
    OPCodes[0x55] = NULL;
    OPCodes[0x56] = &c_DMGCPU::OPCode0x56;
    OPCodes[0x57] = &c_DMGCPU::OPCode0x57;
    OPCodes[0x58] = NULL;
    OPCodes[0x59] = NULL;
    OPCodes[0x5A] = NULL;
    OPCodes[0x5B] = NULL;
    OPCodes[0x5C] = NULL;
    OPCodes[0x5D] = &c_DMGCPU::OPCode0x5D;
    OPCodes[0x5E] = &c_DMGCPU::OPCode0x5E;
    OPCodes[0x5F] = &c_DMGCPU::OPCode0x5F;
    OPCodes[0x60] = &c_DMGCPU::OPCode0x60;
    OPCodes[0x61] = NULL;
    OPCodes[0x62] = NULL;
    OPCodes[0x63] = NULL;
    OPCodes[0x64] = NULL;
    OPCodes[0x65] = NULL;
    OPCodes[0x66] = NULL;
    OPCodes[0x67] = &c_DMGCPU::OPCode0x67;
    OPCodes[0x68] = NULL;
    OPCodes[0x69] = &c_DMGCPU::OPCode0x69;
    OPCodes[0x6A] = NULL;
    OPCodes[0x6B] = NULL;
    OPCodes[0x6C] = NULL;
    OPCodes[0x6D] = NULL;
    OPCodes[0x6E] = NULL;
    OPCodes[0x6F] = &c_DMGCPU::OPCode0x6F;
    OPCodes[0x70] = &c_DMGCPU::OPCode0x70;
    OPCodes[0x71] = &c_DMGCPU::OPCode0x71;
    OPCodes[0x72] = NULL;
    OPCodes[0x73] = NULL;
    OPCodes[0x74] = NULL;
    OPCodes[0x75] = NULL;
    OPCodes[0x76] = NULL;
    OPCodes[0x77] = &c_DMGCPU::OPCode0x77;
    OPCodes[0x78] = &c_DMGCPU::OPCode0x78;
    OPCodes[0x79] = &c_DMGCPU::OPCode0x79;
    OPCodes[0x7A] = &c_DMGCPU::OPCode0x7A;
    OPCodes[0x7B] = &c_DMGCPU::OPCode0x7B;
    OPCodes[0x7C] = &c_DMGCPU::OPCode0x7C;
    OPCodes[0x7D] = &c_DMGCPU::OPCode0x7D;
    OPCodes[0x7E] = &c_DMGCPU::OPCode0x7E;
    OPCodes[0x7F] = NULL;
    OPCodes[0x80] = &c_DMGCPU::OPCode0x80;
    OPCodes[0x81] = &c_DMGCPU::OPCode0x81;
    OPCodes[0x82] = &c_DMGCPU::OPCode0x82;
    OPCodes[0x83] = NULL;
    OPCodes[0x84] = &c_DMGCPU::OPCode0x84;
    OPCodes[0x85] = NULL;
    OPCodes[0x86] = &c_DMGCPU::OPCode0x86;
    OPCodes[0x87] = &c_DMGCPU::OPCode0x87;
    OPCodes[0x88] = &c_DMGCPU::OPCode0x88;
    OPCodes[0x89] = &c_DMGCPU::OPCode0x89;
    OPCodes[0x8A] = NULL;
    OPCodes[0x8B] = NULL;
    OPCodes[0x8C] = NULL;
    OPCodes[0x8D] = NULL;
    OPCodes[0x8E] = NULL;
    OPCodes[0x8F] = NULL;
    OPCodes[0x90] = &c_DMGCPU::OPCode0x90;
    OPCodes[0x91] = NULL;
    OPCodes[0x92] = &c_DMGCPU::OPCode0x92;
    OPCodes[0x93] = NULL;
    OPCodes[0x94] = NULL;
    OPCodes[0x95] = NULL;
    OPCodes[0x96] = &c_DMGCPU::OPCode0x96;
    OPCodes[0x97] = &c_DMGCPU::OPCode0x97;
    OPCodes[0x98] = NULL;
    OPCodes[0x99] = &c_DMGCPU::OPCode0x99;
    OPCodes[0x9A] = NULL;
    OPCodes[0x9B] = NULL;
    OPCodes[0x9C] = NULL;
    OPCodes[0x9D] = NULL;
    OPCodes[0x9E] = NULL;
    OPCodes[0x9F] = NULL;
    OPCodes[0xA0] = &c_DMGCPU::OPCode0xA0;
    OPCodes[0xA1] = &c_DMGCPU::OPCode0xA1;
    OPCodes[0xA2] = NULL;
    OPCodes[0xA3] = NULL;
    OPCodes[0xA4] = NULL;
    OPCodes[0xA5] = NULL;
    OPCodes[0xA6] = NULL;
    OPCodes[0xA7] = &c_DMGCPU::OPCode0xA7;
    OPCodes[0xA8] = NULL;
    OPCodes[0xA9] = &c_DMGCPU::OPCode0xA9;
    OPCodes[0xAA] = NULL;
    OPCodes[0xAB] = NULL;
    OPCodes[0xAC] = NULL;
    OPCodes[0xAD] = NULL;
    OPCodes[0xAE] = &c_DMGCPU::OPCode0xAE;
    OPCodes[0xAF] = &c_DMGCPU::OPCode0xAF;
    OPCodes[0xB0] = &c_DMGCPU::OPCode0xB0;
    OPCodes[0xB1] = &c_DMGCPU::OPCode0xB1;
    OPCodes[0xB2] = NULL;
    OPCodes[0xB3] = &c_DMGCPU::OPCode0xB3;
    OPCodes[0xB4] = NULL;
    OPCodes[0xB5] = NULL;
    OPCodes[0xB6] = NULL;
    OPCodes[0xB7] = &c_DMGCPU::OPCode0xB7;
    OPCodes[0xB8] = NULL;
    OPCodes[0xB9] = NULL;
    OPCodes[0xBA] = NULL;
    OPCodes[0xBB] = NULL;
    OPCodes[0xBC] = NULL;
    OPCodes[0xBD] = NULL;
    OPCodes[0xBE] = &c_DMGCPU::OPCode0xBE;
    OPCodes[0xBF] = NULL;
    OPCodes[0xC0] = &c_DMGCPU::OPCode0xC0;
    OPCodes[0xC1] = &c_DMGCPU::OPCode0xC1;
    OPCodes[0xC2] = &c_DMGCPU::OPCode0xC2;
    OPCodes[0xC3] = &c_DMGCPU::OPCode0xC3;
    OPCodes[0xC4] = &c_DMGCPU::OPCode0xC4;
    OPCodes[0xC5] = &c_DMGCPU::OPCode0xC5;
    OPCodes[0xC6] = &c_DMGCPU::OPCode0xC6;
    OPCodes[0xC7] = NULL;
    OPCodes[0xC8] = &c_DMGCPU::OPCode0xC8;
    OPCodes[0xC9] = &c_DMGCPU::OPCode0xC9;
    OPCodes[0xCA] = &c_DMGCPU::OPCode0xCA;
    //Function to read a CB opcode, not an actual opcode in itself
    //There is a switch to check if it's a CB opcode now. I think
    //we're better making a separate opcode table for them.
    OPCodes[0xCC] = NULL;
    OPCodes[0xCD] = &c_DMGCPU::OPCode0xCD;
    OPCodes[0xCE] = NULL;
    OPCodes[0xCF] = NULL;
    OPCodes[0xD0] = &c_DMGCPU::OPCode0xD0;
    OPCodes[0xD1] = &c_DMGCPU::OPCode0xD1;
    OPCodes[0xD2] = NULL;
    OPCodes[0xD3] = NULL;
    OPCodes[0xD4] = NULL;
    OPCodes[0xD5] = &c_DMGCPU::OPCode0xD5;
    OPCodes[0xD6] = &c_DMGCPU::OPCode0xD6;
    OPCodes[0xD7] = NULL;
    OPCodes[0xD8] = NULL;
    OPCodes[0xD9] = &c_DMGCPU::OPCode0xD9;
    OPCodes[0xDA] = &c_DMGCPU::OPCode0xDA;
    OPCodes[0xDB] = NULL;
    OPCodes[0xDC] = NULL;
    OPCodes[0xDD] = NULL;
    OPCodes[0xDE] = &c_DMGCPU::OPCode0xDE;
    OPCodes[0xDF] = NULL;
    OPCodes[0xE0] = &c_DMGCPU::OPCode0xE0;
    OPCodes[0xE1] = &c_DMGCPU::OPCode0xE1;
    OPCodes[0xE2] = &c_DMGCPU::OPCode0xE2;
    OPCodes[0xE3] = NULL;
    OPCodes[0xE4] = NULL;
    OPCodes[0xE5] = &c_DMGCPU::OPCode0xE5;
    OPCodes[0xE6] = &c_DMGCPU::OPCode0xE6;
    OPCodes[0xE7] = NULL;
    OPCodes[0xE8] = &c_DMGCPU::OPCode0xE8;
    OPCodes[0xE9] = &c_DMGCPU::OPCode0xE9;
    OPCodes[0xEA] = &c_DMGCPU::OPCode0xEA;
    OPCodes[0xEB] = NULL;
    OPCodes[0xEC] = NULL;
    OPCodes[0xED] = NULL;
    OPCodes[0xEE] = NULL;
    OPCodes[0xEF] = &c_DMGCPU::OPCode0xEF;
    OPCodes[0xF0] = &c_DMGCPU::OPCode0xF0;
    OPCodes[0xF1] = &c_DMGCPU::OPCode0xF1;
    OPCodes[0xF2] = NULL;
    OPCodes[0xF3] = &c_DMGCPU::OPCode0xF3;
    OPCodes[0xF4] = NULL;
    OPCodes[0xF5] = &c_DMGCPU::OPCode0xF5;
    OPCodes[0xF6] = NULL;
    OPCodes[0xF7] = NULL;
    OPCodes[0xF8] = NULL;
    OPCodes[0xF9] = NULL;
    OPCodes[0xFA] = &c_DMGCPU::OPCode0xFA;
    OPCodes[0xFB] = &c_DMGCPU::OPCode0xFB;
    OPCodes[0xFC] = NULL;
    OPCodes[0xFD] = NULL;
    OPCodes[0xFE] = &c_DMGCPU::OPCode0xFE;
    OPCodes[0xFF] = NULL;

    //CB opcode table. Make sure to update tick function when opcodes are implemented.
    uint8_t i;
    for(i = 0; i < 0xFF; i++)
    {
        OPCodesCB[i] = NULL; //Initialise table with nulls to stop it crashing on unknowns.
    }
    OPCodesCB[0x11] = &c_DMGCPU::OPCodeCB0x11;
    OPCodesCB[0x17] = &c_DMGCPU::OPCodeCB0x17;
    OPCodesCB[0x23] = &c_DMGCPU::OPCodeCB0x23;
    OPCodesCB[0x27] = &c_DMGCPU::OPCodeCB0x27;
    OPCodesCB[0x37] = &c_DMGCPU::OPCodeCB0x37;
    OPCodesCB[0x38] = &c_DMGCPU::OPCodeCB0x38;
    OPCodesCB[0x41] = &c_DMGCPU::OPCodeCB0x41;
    OPCodesCB[0x47] = &c_DMGCPU::OPCodeCB0x47;
    OPCodesCB[0x67] = &c_DMGCPU::OPCodeCB0x67;
    OPCodesCB[0x6F] = &c_DMGCPU::OPCodeCB0x6F;
    OPCodesCB[0x77] = &c_DMGCPU::OPCodeCB0x77;
    OPCodesCB[0x7C] = &c_DMGCPU::OPCodeCB0x7C;
    OPCodesCB[0x7F] = &c_DMGCPU::OPCodeCB0x7F;
    OPCodesCB[0xFE] = &c_DMGCPU::OPCodeCB0xFE;

}

void c_DMGCPU::IllegalOperation(uint8_t opcode, bool iscb)
{
    running = false;
    DbgOut(DBG_CPU, VERBOSE_0, "-------------------------------------------------");

    if(iscb)
    {
        DbgOut(DBG_CPU, VERBOSE_0, "Unknown opcode: 0xCB%x. PC: 0x%x", MMU->ReadByte(Registers.PC.word+1), Registers.PC.word);
    }
    else {
        DbgOut(DBG_CPU, VERBOSE_0, "Unknown opcode: 0x%x. PC: 0x%x", MMU->ReadByte(Registers.PC.word), Registers.PC.word);
    }

    //Dump Registers.
    DbgOut(DBG_CPU, VERBOSE_0, "-------------------------------------------------");
    DbgOut(DBG_CPU, VERBOSE_0, "Register Dump");
    DbgOut(DBG_CPU, VERBOSE_0, "-------------------------------------------------");
    DbgOut(DBG_CPU, VERBOSE_0, "A = 0x%x", Registers.AF.hi);
    DbgOut(DBG_CPU, VERBOSE_0, "B = 0x%x", Registers.BC.hi);
    DbgOut(DBG_CPU, VERBOSE_0, "C = 0x%x", Registers.BC.lo);
    DbgOut(DBG_CPU, VERBOSE_0, "D = 0x%x", Registers.DE.hi);
    DbgOut(DBG_CPU, VERBOSE_0, "E = 0x%x", Registers.DE.lo);
    DbgOut(DBG_CPU, VERBOSE_0, "F = 0x%x", Registers.AF.lo);
    DbgOut(DBG_CPU, VERBOSE_0, "H = 0x%x", Registers.HL.hi);
    DbgOut(DBG_CPU, VERBOSE_0, "L = 0x%x", Registers.HL.lo);
    DbgOut(DBG_CPU, VERBOSE_0, "PC = 0x%x", Registers.PC.word);
    DbgOut(DBG_CPU, VERBOSE_0, "SP = 0x%x", Registers.SP.word);
    DbgOut(DBG_CPU, VERBOSE_0, "-------------------------------------------------");
}

//NOP Instruction - Temporarily using as unknown opcode. BIOS doesn't use NOP.
void c_DMGCPU::OPCode0x00()
{
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load immediate 16-bit value into BC
void c_DMGCPU::OPCode0x01()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD BC, d16");
    Registers.BC.word = MMU->ReadWord(Registers.PC.word + 1);
    Clock.m = 3;
    Clock.t = 12;
    Registers.PC.word += 3;
}

//Load A into the address stored in BC
void c_DMGCPU::OPCode0x02()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (BC), A");
    MMU->WriteByte(Registers.BC.word, Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment BC
void c_DMGCPU::OPCode0x03()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC BC");
    Registers.BC.word++;
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment B
void c_DMGCPU::OPCode0x04()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC B");

    uint8_t result = Registers.BC.hi + 1;

    if(result == 0)
        SET_FLAG_BIT(FLAG_ZERO);
    else
        UNSET_FLAG_BIT(FLAG_ZERO);

    UNSET_FLAG_BIT(SUB_BIT);

    if((Registers.BC.hi & 0xF) == 0xF)
        SET_FLAG_BIT(FLAG_HC);
    else
        UNSET_FLAG_BIT(FLAG_HC);

    Registers.BC.hi = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement B
void c_DMGCPU::OPCode0x05()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC B");
    SET_FLAG_BIT(SUB_BIT);

    Registers.BC.hi--;

    if((Registers.BC.hi & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.BC.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load 8-bit immediate value into B
void c_DMGCPU::OPCode0x06()
{
    Registers.BC.hi = MMU->ReadByte(Registers.PC.word + 1);
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, d8. B = 0x%x", Registers.BC.hi);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Rotate A left through Carry bit (Bit 7 to carry and Carry bit)
void c_DMGCPU::OPCode0x07()
{
    uint8_t carryi = (FLAG_CARRY ? 1 : 0);

    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);


    if(MSB(Registers.AF.hi))
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Registers.AF.hi = Registers.AF.hi << 1;
    Registers.AF.hi |= carryi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load the StackPointer into the address specified by immediate 16-bit value
void c_DMGCPU::OPCode0x08()
{
    MMU->WriteWord(Registers.SP.word, MMU->ReadWord(Registers.PC.word++));
    Clock.m = 4; //Is this correct amount of machine cycles??
    Clock.t = 20;
    Registers.PC.word += 3;
}

//Add BC to HL and store in HL
void c_DMGCPU::OPCode0x09()
{
    UNSET_FLAG_BIT(SUB_BIT);

    if((Registers.HL.word + Registers.BC.word) >= 0x10000)
        SET_FLAG_BIT(CARRY_BIT);

    if(((Registers.HL.word + Registers.BC.word) & 0xFFF) < (Registers.HL.word & 0xFFF))
        SET_FLAG_BIT(HC_BIT);

    Registers.HL.word += Registers.BC.word;

    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Load value pointed to by BC into A
void c_DMGCPU::OPCode0x0A()
{
    Registers.AF.hi = MMU->ReadByte(Registers.BC.word);
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (BC), A = 0x%x", Registers.AF.hi);
    Clock.m = 3;
    Clock.t = 8;
    Registers.PC.word++;
}

//Decrement B
void c_DMGCPU::OPCode0x0B()
{
    Registers.BC.word--;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Increment C
void c_DMGCPU::OPCode0x0C()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC C");

    uint8_t result = Registers.BC.lo + 1;

    if(result == 0)
        SET_FLAG_BIT(FLAG_ZERO);
    else
        UNSET_FLAG_BIT(FLAG_ZERO);

    UNSET_FLAG_BIT(SUB_BIT);

    if((result & 0xF) == 0)
        SET_FLAG_BIT(FLAG_HC);
    else
        UNSET_FLAG_BIT(FLAG_HC);

    Registers.BC.lo = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement C
void c_DMGCPU::OPCode0x0D()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC C");
    SET_FLAG_BIT(SUB_BIT);

    if((Registers.BC.lo & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.BC.lo--;

    if(Registers.BC.lo == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load immediate 8-bit value into C.
void c_DMGCPU::OPCode0x0E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, d8");
    Registers.BC.lo = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Rotate A right. Store old bit 0 in carry.
void c_DMGCPU::OPCode0x0F()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RRCA");
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    if(LSB(Registers.AF.hi))
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    uint8_t carryn = (FLAG_CARRY ? 1 : 0);

    Registers.AF.hi = Registers.AF.hi >> 1;
    Registers.AF.hi |= (carryn << 7);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//STOP 0
void c_DMGCPU::OPCode0x10()
{

    //According to http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    //"Instruction STOP has according to manuals opcode 10 00 and thus is 2 bytes long. Anyhow it seems there is no reason for it so some assemblers code it simply as one byte instruction 10."

    Clock.m = 1;
    Clock.t = 4;

    //Sit around and do nothing until either a button is pressed (stored in A) or the joypad is presssed
    while(Registers.AF.hi == 0 || MMU->ReadByte(0xFF00) == 0);;

    if(MMU->ReadByte(Registers.PC.word + 1) == 0x00)
        Registers.PC.word += 2;
    else
        Registers.PC.word++;
}

//Load immediate 16-bit value into DE.
void c_DMGCPU::OPCode0x11()
{
    Registers.DE.word = MMU->ReadWord(Registers.PC.word + 1);
    DbgOut(DBG_CPU, VERBOSE_2, "LD DE, d16. DE = 0x%x", Registers.DE.word);
    Registers.PC.word += 3;
    Clock.m = 3;
    Clock.t = 12;
}

//Load A into the address pointed to by DE
void c_DMGCPU::OPCode0x12()
{
    //Load doesn't affect flags
    DbgOut(DBG_CPU, VERBOSE_2, "LD (DE), A A=0x%x, DE=0x%x", Registers.AF.hi, Registers.DE.word);
    MMU->WriteByte(Registers.DE.word, Registers.AF.hi);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment DE
void c_DMGCPU::OPCode0x13()
{
    //Flags not affected because this is a 16-bit addition.
    Registers.DE.word++;
    DbgOut(DBG_CPU, VERBOSE_2, "INC DE. DE = 0x%x", Registers.DE.word);
    Registers.PC.word++;
    Clock.m = 1;
    Clock.t = 8;
}

//Increment D
void c_DMGCPU::OPCode0x14()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC D");
    UNSET_FLAG_BIT(SUB_BIT);

    if((Registers.DE.hi & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);


    Registers.DE.hi++;

    if(Registers.DE.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement D
void c_DMGCPU::OPCode0x15()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC D");
    SET_FLAG_BIT(SUB_BIT);

    Registers.DE.hi--;

    if((Registers.DE.hi & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.DE.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load 8-bit immediate value into D
void c_DMGCPU::OPCode0x16()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD D, d8");
    Registers.DE.hi = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 4;
    Registers.PC.word += 2; //2-byte instruction
}

//Rotate A left. Bit 7 to Carry, Carry bit to Bit 0
void c_DMGCPU::OPCode0x17()
{
    //Store carry flag.
    uint8_t carryi = (FLAG_CARRY ? 1 : 0);
    uint8_t regi = Registers.AF.hi;

    //Unset flag bits
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);


    if(MSB(Registers.AF.hi) > 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);


    regi = (regi << 1) | carryi;

    DbgOut(DBG_CPU, VERBOSE_2, "RL A. Aorig = 0x%x. Anow= 0x%x carryi = %i.", Registers.AF.hi, regi, carryi);
    Registers.AF.hi = regi;

    Clock.t = 1;
    Clock.m = 4;
    //Needs to be changed.
    Registers.PC.word += 1;
}

//Jump relative by adding n bytes to the current program counter
void c_DMGCPU::OPCode0x18()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JR r8");
    Registers.PC.word += (int8_t)MMU->ReadByte(Registers.PC.word + 1) + 2; //Signed 8-bit value
    Clock.m = 3;
    Clock.t = 4;
}

//Add DE to HL, store in HL
void c_DMGCPU::OPCode0x19()
{
    DbgOut(DBG_CPU, VERBOSE_2, "ADD HL, DE");

    UNSET_FLAG_BIT(SUB_BIT);

    uint16_t result = Registers.HL.word + Registers.DE.word;

    if((result & 0xFFF) < (Registers.HL.word & 0xFFF));

    Registers.HL.word = result;

    if((Registers.HL.word + Registers.DE.word) > 0xFFFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++; //1 byte instruction
}

//Load value pointed to by DE into A.
void c_DMGCPU::OPCode0x1A()
{
    Registers.AF.hi = MMU->ReadByte(Registers.DE.word);
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (DE). A = 0x%x", Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Decrement DE
void c_DMGCPU::OPCode0x1B()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC DE");
    Registers.DE.word--;
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++; //1-byte instruction
}

//Increment E
void c_DMGCPU::OPCode0x1C()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC E");

    uint8_t result = Registers.DE.lo + 1;

    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((result & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if((Registers.DE.lo + 1) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Registers.DE.lo = result;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement E
void c_DMGCPU::OPCode0x1D()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC E");

    SET_FLAG_BIT(SUB_BIT); //We are performing a subtraction!

    uint8_t result = Registers.DE.lo - 1;

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(result > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.DE.lo = result; //Actually perform the subtraction

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load immediate 8-bit value into E.
void c_DMGCPU::OPCode0x1E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD E, d8");
    Registers.DE.lo = MMU->ReadByte(Registers.PC.word+1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Rotate A right. Bit 0 to Carry, Carry bit to Bit 7
void c_DMGCPU::OPCode0x1F()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RRA");
    //Unset Flag bits
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    //MSB of the A register
    uint8_t bit0 = LSB(Registers.AF.hi);
    uint8_t cflag = FLAG_CARRY; //Carry at this point in time (not AFTER the shift)

    if(bit0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Registers.AF.hi  = Registers.AF.hi >> 1;
    Registers.AF.hi |= cflag << 7;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Relative jump if ZERO flag is not set. Add immediate (signed) value to PC.
void c_DMGCPU::OPCode0x20()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JR NZ, d8");
    if(!FLAG_ZERO)
    {
        //Zero flag is set. Jump.
        Registers.PC.word += (int8_t)MMU->ReadByte(Registers.PC.word + 1) + 2;
        DbgOut(DBG_CPU, VERBOSE_2, "Zero bit not set. Jumping to 0x%x", Registers.PC.word);
        Clock.t = 12;
    }
    else {
        Registers.PC.word += 2;
        DbgOut(DBG_CPU, VERBOSE_2, "Zero bit set, not jumping.");
        Clock.t = 8;
    }
    Clock.m = 2;
}

//Load immediate 16-bit value into HL.
void c_DMGCPU::OPCode0x21()
{
    Registers.HL.word = MMU->ReadWord(Registers.PC.word + 1);
    DbgOut(DBG_CPU, VERBOSE_2, "LD HL, d16. HL = 0x%x", Registers.HL.word);
    Registers.PC.word += 3;
    Clock.m = 3;
    Clock.t = 12;
}

void c_DMGCPU::OPCode0x22()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL+), A");
    MMU->WriteByte(Registers.HL.word, Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.HL.word++;
    Registers.PC.word++;
}

//Increment HL
void c_DMGCPU::OPCode0x23()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC HL");
    Registers.HL.word++;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Increment H
//Using variable++ anywhere will increment the variable by 1.
void c_DMGCPU::OPCode0x24()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC H");

    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition

    uint8_t result = Registers.HL.hi + 1;

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((result & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if((Registers.HL.hi + 1) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Registers.HL.hi = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++; //1 byte instruction
}

//Decrement H
void c_DMGCPU::OPCode0x25()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC H");

    SET_FLAG_BIT(SUB_BIT); //We are performing a subtraction.

    uint8_t result = Registers.HL.hi - 1;

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((result & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.HL.hi = result;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++; //1 byte instruction
}

//Load 8-bit immediate value into H
void c_DMGCPU::OPCode0x26()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD H, d8");

    Registers.HL.hi = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 byte instruction
}

//Decimal Adjust A
void c_DMGCPU::OPCode0x27()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DAA (Decimal Adjust A)");

    uint8_t a = Registers.AF.hi;

    if(FLAG_SUB)
    {
        if(FLAG_HC)
            a = (a - 0x06) & 0xFF;
        if(FLAG_CARRY)
            a -= 0x60;
    }
    else
    {
        if(FLAG_HC || (a & 0xF) > 9)
            a += 0x06;
        if(FLAG_CARRY || a > 0x9F)
            a += 0x60;
    }

    Registers.AF.hi = a;
    UNSET_FLAG_BIT(HC_BIT);

    if(!Registers.AF.hi)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(a >= 0x100)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Jump relative if zero bit is set.
void c_DMGCPU::OPCode0x28()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JR Z, d8");
    if(FLAG_ZERO)
    {
        //Zero flag is set. Jump.
        Registers.PC.word += (int8_t)(MMU->ReadByte(Registers.PC.word + 1) + 2);
        DbgOut(DBG_CPU, VERBOSE_0, "Zero bit set. Jumping to 0x%x", Registers.PC.word);
        Clock.t = 12;
    }
    else {
        Registers.PC.word += 2;
        DbgOut(DBG_CPU, VERBOSE_2, "Zero bit not set, not jumping.");
        Clock.t = 8;
    }
    Clock.m = 2;
}

//Add HL to HL and store in HL
void c_DMGCPU::OPCode0x29()
{
    DbgOut(DBG_CPU, VERBOSE_2, "ADD HL, HL");

    UNSET_FLAG_BIT(SUB_BIT);

    if((Registers.HL.word + Registers.HL.word) > 0xFFFF)
        SET_FLAG_BIT(CARRY_BIT);

    if((Registers.HL.word + Registers.HL.word) > 0xFF)
        SET_FLAG_BIT(HC_BIT);

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 byte instruction
}

//Load value pointed to by HL into A, then increment HL.
void c_DMGCPU::OPCode0x2A()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (HL+)");

    Registers.AF.hi = MMU->ReadByte(Registers.HL.word);

    Registers.HL.word++;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++; //1 byte instruction
}

//Decrement HL
void c_DMGCPU::OPCode0x2B()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC HL");

    Registers.HL.word--;
    Clock.m = 1;
    Clock.t = 8;

    Registers.PC.word++;
}

//Increment L
void c_DMGCPU::OPCode0x2C()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition!

    DbgOut(DBG_CPU, VERBOSE_2, "DEC L, L = 0x%x", Registers.HL.lo);

    uint8_t result = Registers.HL.lo + 1;

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((result & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if((Registers.HL.lo + 1) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Registers.HL.lo = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement L
void c_DMGCPU::OPCode0x2D()
{
    SET_FLAG_BIT(SUB_BIT);

    if((Registers.HL.lo & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.HL.lo--;

    DbgOut(DBG_CPU, VERBOSE_2, "DEC L, L = 0x%x", Registers.HL.lo);

    if(Registers.HL.lo == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load immediate 8-bit value into L.
void c_DMGCPU::OPCode0x2E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD L, d8");
    Registers.HL.lo = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Compliment A. Flip all of A's bits
void c_DMGCPU::OPCode0x2F()
{
    SET_FLAG_BIT(SUB_BIT);
    SET_FLAG_BIT(HC_BIT);
    Registers.AF.hi = ~Registers.AF.hi; //Invert A
    DbgOut(DBG_CPU, VERBOSE_2, "CPL A");
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Jump relative by adding n-bytes to the Program Counter (signed) if the Carry flag is unset
void c_DMGCPU::OPCode0x30()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JR NC, r8");
    if(!FLAG_CARRY)
    {
        Registers.PC.word += (int8_t)MMU->ReadByte(Registers.PC.word + 1) + 2; //Signed 8-bit value
        DbgOut(DBG_CPU, VERBOSE_2, "Carry bit not set. Jumping to 0x%x", Registers.PC.word);
        Clock.t = 12;
    }
    else
    {
        Registers.PC.word += 2; //2 byte instruction
        Clock.t = 8;
    }

    Clock.m = 3;
}

//Load immediate 16-bit value into SP.
void c_DMGCPU::OPCode0x31()
{
    Registers.SP.word = MMU->ReadWord(Registers.PC.word + 1);
    DbgOut(DBG_CPU, VERBOSE_2, "LD SP, d16. SP = 0x%x", Registers.SP.word);
    Registers.PC.word += 3;
    Clock.m = 3;
    Clock.t = 12;
}

//Load 8-bit value in A into memory location pointed to by HL. Decrement HL.
void c_DMGCPU::OPCode0x32()
{
    MMU->WriteByte(Registers.HL.word, Registers.AF.hi);
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL-), A. HL = 0x%x. A = 0x%x", Registers.HL.word, Registers.AF.hi);
    Registers.HL.word--;
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment the Stack Pointer
void c_DMGCPU::OPCode0x33()
{
    Registers.SP.word++;
    DbgOut(DBG_CPU, VERBOSE_2, "INC SP. SP = 0x%x", Registers.SP.word);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment value pointed to by HL
void c_DMGCPU::OPCode0x34()
{
    UNSET_FLAG_BIT(SUB_BIT);

    uint8_t value = MMU->ReadByte(Registers.HL.word);
    value++;
    DbgOut(DBG_CPU, VERBOSE_2, "INC (HL) (HL) = 0x%x", value);

    if(value == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((value & 0xF) == 0)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    MMU->WriteByte(Registers.HL.word, value);

    Clock.m = 3;
    Clock.t = 12;
    Registers.PC.word++;
}

//Decrement Value pointed to by HL
void c_DMGCPU::OPCode0x35()
{
    UNSET_FLAG_BIT(SUB_BIT);

    uint8_t value = MMU->ReadByte(Registers.HL.word);
    value--;
    DbgOut(DBG_CPU, VERBOSE_2, "DEC (HL) (HL) = 0x%x", value);

    if(value == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((value & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    MMU->WriteByte(Registers.HL.word, value);

    Clock.m = 3;
    Clock.t = 12;
    Registers.PC.word++;
}

//Load immediate 8-bit value into pointer HL.
void c_DMGCPU::OPCode0x36()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL), d8");
    MMU->WriteByte(Registers.HL.word, MMU->ReadByte(Registers.PC.word+1));
    Clock.m = 2;
    Clock.t = 12;
    Registers.PC.word += 2;
}

//Set carry flag.
void c_DMGCPU::OPCode0x37()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SCF");
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    SET_FLAG_BIT(CARRY_BIT);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Relative jump if carry flag is set. Add immediate (signed) value to PC.
void c_DMGCPU::OPCode0x38()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JR C, d8");
    if(FLAG_CARRY)
    {
        //Zero flag is set. Jump.
        Registers.PC.word += (int8_t)MMU->ReadByte(Registers.PC.word + 1) + 2;
        DbgOut(DBG_CPU, VERBOSE_2, "Carry bit set. Jumping to 0x%x", Registers.PC.word);
        Clock.t = 12;
    }
    else {
        Registers.PC.word += 2;
        DbgOut(DBG_CPU, VERBOSE_2, "Carry bit not set, not jumping.");
        Clock.t = 8;
    }
    Clock.m = 2;
}

//Add SP to HL.
void c_DMGCPU::OPCode0x39()
{
    DbgOut(DBG_CPU, VERBOSE_2, "Add HL, SP");
    //Zero flag is not affected.
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    if((Registers.HL.word + Registers.SP.word) > 0xFFFF)
        SET_FLAG_BIT(CARRY_BIT);

    if((Registers.HL.word + Registers.SP.word) > 0xFF)
        SET_FLAG_BIT(HC_BIT);

    Registers.HL.word += Registers.SP.word;

    Clock.m = 2;
    Clock.t = 16;
    Registers.PC.word++;
}

//Load value pointed to by HL into A then decrement HL.
void c_DMGCPU::OPCode0x3A()
{
    Registers.AF.hi = MMU->ReadByte(Registers.HL.word);
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (HL-), A = 0x%x", Registers.AF.hi);
    Registers.HL.word--;
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Decrement Stack pointer
void c_DMGCPU::OPCode0x3B()
{
    Registers.SP.word++;
    DbgOut(DBG_CPU, VERBOSE_2, "INC SP, SP = 0x%x", Registers.SP.word);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//Increment A
void c_DMGCPU::OPCode0x3C()
{
    DbgOut(DBG_CPU, VERBOSE_2, "INC A");

    uint8_t result = Registers.AF.hi + 1;

    if(result == 0)
        SET_FLAG_BIT(FLAG_ZERO);
    else
        UNSET_FLAG_BIT(FLAG_ZERO);

    UNSET_FLAG_BIT(SUB_BIT);

    if((result & 0xF) > 0xF)
        SET_FLAG_BIT(FLAG_HC);
    else
        UNSET_FLAG_BIT(FLAG_HC);

    if((Registers.AF.hi + 1) > 0xFF)
        SET_FLAG_BIT(FLAG_CARRY);
    else
        UNSET_FLAG_BIT(FLAG_CARRY);

    Registers.AF.hi = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Decrement A
void c_DMGCPU::OPCode0x3D()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DEC A");
    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi--;

    if((Registers.AF.hi & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load immediate 8-bit value into A.
void c_DMGCPU::OPCode0x3E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, d8");
    Registers.AF.hi = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Compliment the carry flag, if it's set, reset it, if not set it
void c_DMGCPU::OPCode0x3F()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    DbgOut(DBG_CPU, VERBOSE_2, "CCF");
    if(FLAG_CARRY)
    {
        DbgOut(DBG_CPU, VERBOSE_2, "Carry bit set! Restting it...");
        UNSET_FLAG_BIT(CARRY_BIT);

    }
    else{
        DbgOut(DBG_CPU, VERBOSE_2, "Carry bit not set! Setting...");
        SET_FLAG_BIT(CARRY_BIT);
    }

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load B into B
void c_DMGCPU::OPCode0x40()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, B");
    Registers.BC.hi = Registers.BC.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load C into B
void c_DMGCPU::OPCode0x41()
{
    Registers.BC.hi = Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, C. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load D into B
void c_DMGCPU::OPCode0x42()
{
    Registers.BC.hi = Registers.DE.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, D. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load E into B
void c_DMGCPU::OPCode0x43()
{
    Registers.BC.hi = Registers.DE.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, E. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load H into B
void c_DMGCPU::OPCode0x44()
{
    Registers.BC.hi = Registers.HL.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, H. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load L into B
void c_DMGCPU::OPCode0x45()
{
    Registers.BC.hi = Registers.HL.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, L. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load value pointed to by HL into B
void c_DMGCPU::OPCode0x46()
{
    Registers.BC.hi = MMU->ReadByte(Registers.HL.word);
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, (HL). B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load A into B
void c_DMGCPU::OPCode0x47()
{
    Registers.BC.hi = Registers.AF.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD B, A. B = 0x%x", Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load B into C
void c_DMGCPU::OPCode0x48()
{
    Registers.BC.lo = Registers.BC.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, B. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load C into C
void c_DMGCPU::OPCode0x49()
{
    Registers.BC.lo = Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, C. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load D into C
void c_DMGCPU::OPCode0x4A()
{
    Registers.BC.lo = Registers.DE.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, D. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load E into C
void c_DMGCPU::OPCode0x4B()
{
    Registers.BC.lo = Registers.DE.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, E. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load H into C
void c_DMGCPU::OPCode0x4C()
{
    Registers.BC.lo = Registers.HL.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, H. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load L into C
void c_DMGCPU::OPCode0x4D()
{
    Registers.BC.lo = Registers.HL.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, L. C = 0x%x", Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load Value pointed by HL into C
void c_DMGCPU::OPCode0x4E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, (HL)");
    Registers.BC.lo = MMU->ReadByte(Registers.HL.word);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load H into D.
void c_DMGCPU::OPCode0x54()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD D, H");
    Registers.DE.hi = Registers.HL.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load value pointed to by HL into D
void c_DMGCPU::OPCode0x56()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD D, (HL)");
    Registers.DE.hi = MMU->ReadByte(Registers.HL.word);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load A into D.
void c_DMGCPU::OPCode0x57()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD D, A");
    Registers.DE.hi = Registers.AF.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load L into E.
void c_DMGCPU::OPCode0x5D()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD E, L");
    Registers.DE.lo = Registers.HL.lo;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}


//Load Value pointed by HL into E
void c_DMGCPU::OPCode0x5E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD E, (HL)");
    Registers.DE.lo = MMU->ReadByte(Registers.HL.word);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load A into E
void c_DMGCPU::OPCode0x5F()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD E, A");
    Registers.DE.lo = Registers.AF.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load B into H.
void c_DMGCPU::OPCode0x60()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD H, B");
    Registers.HL.hi = Registers.BC.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load A into H.
void c_DMGCPU::OPCode0x67()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD H, A");
    Registers.HL.hi = Registers.AF.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load C into L.
void c_DMGCPU::OPCode0x69()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD L, C");
    Registers.HL.lo = Registers.BC.lo;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load A into L.
void c_DMGCPU::OPCode0x6F()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD L, A");
    Registers.HL.lo = Registers.AF.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Store 8-bit value in register B into pointer HL.
void c_DMGCPU::OPCode0x70()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL), B");
    MMU->WriteByte(Registers.HL.word, Registers.BC.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Store 8-bit value in register C into pointer HL.
void c_DMGCPU::OPCode0x71()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL), C");
    MMU->WriteByte(Registers.HL.word, Registers.BC.lo);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Store 8-bit value in register A into pointer HL.
void c_DMGCPU::OPCode0x77()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (HL), A");
    MMU->WriteByte(Registers.HL.word, Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Load C into A
void c_DMGCPU::OPCode0x79()
{
    Registers.AF.hi = Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, A");
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load D into A
void c_DMGCPU::OPCode0x7A()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, D");
    Registers.AF.hi = Registers.DE.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load E into A.
void c_DMGCPU::OPCode0x7B()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, E");
    Registers.AF.hi = Registers.DE.lo;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load H into A.
void c_DMGCPU::OPCode0x7C()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, H");
    Registers.AF.hi = Registers.HL.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Subtract B from A, then store in A.
void c_DMGCPU::OPCode0x90()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SUB B");

    if((Registers.AF.hi - Registers.BC.hi) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - Registers.BC.hi) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - Registers.BC.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= Registers.BC.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Subtract D from A, then store in A.
void c_DMGCPU::OPCode0x92()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SUB D");

    if((Registers.AF.hi - Registers.DE.hi) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - Registers.DE.hi) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - Registers.DE.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= Registers.DE.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Subtract value stored in (HL) from A.
void c_DMGCPU::OPCode0x96()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SUB (HL)");

    uint8_t operand = MMU->ReadByte(Registers.HL.word);

    if((Registers.AF.hi - operand) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - operand) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - operand == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= operand;

    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}
//Subtract A from A, then store in A.
void c_DMGCPU::OPCode0x97()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SUB A");

    if((Registers.AF.hi - Registers.AF.hi) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - Registers.AF.hi) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= Registers.AF.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Subtract C + Carry flag from A.
void c_DMGCPU::OPCode0x99()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SBC A, C");

    uint8_t carry = (FLAG_CARRY ? 1 : 0);

    uint8_t value = Registers.BC.lo + carry;

    if((Registers.AF.hi - value) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - value) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - value == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= value;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load Register A into Register C
void c_DMGCPU::OPCode0x4F()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD C, A");
    Registers.BC.lo = Registers.AF.hi;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load B into A
void c_DMGCPU::OPCode0x78()
{
    Registers.AF.hi = Registers.BC.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, B");
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load L into A
void c_DMGCPU::OPCode0x7D()
{
    Registers.AF.hi = Registers.HL.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, L");
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load value pointed to by HL into A
void c_DMGCPU::OPCode0x7E()
{
    Registers.AF.hi = MMU->ReadByte(Registers.HL.word);
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (HL), A = 0x%x", Registers.AF.hi);
    Clock.m = 3;
    Clock.t = 8;
    Registers.PC.word++;
}

//ADD B to A
void c_DMGCPU::OPCode0x80()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    DbgOut(DBG_CPU, VERBOSE_2, "ADD A, B");

    if(Registers.AF.hi + Registers.BC.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi + Registers.BC.hi > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(Registers.AF.hi + Registers.BC.hi > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi += Registers.BC.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//ADD C to A
void c_DMGCPU::OPCode0x81()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    DbgOut(DBG_CPU, VERBOSE_2, "ADD A, C");

    if(Registers.AF.hi + Registers.BC.lo == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi + Registers.BC.lo > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(Registers.AF.hi + Registers.BC.lo > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi += Registers.BC.lo;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//ADD D to A
void c_DMGCPU::OPCode0x82()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    DbgOut(DBG_CPU, VERBOSE_2, "ADD A, D");

    if(Registers.AF.hi + Registers.DE.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi + Registers.DE.hi > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(Registers.AF.hi + Registers.DE.hi > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi += Registers.DE.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//ADD H to A
void c_DMGCPU::OPCode0x84()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    DbgOut(DBG_CPU, VERBOSE_2, "ADD A, H");

    if(Registers.AF.hi + Registers.HL.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi + Registers.HL.hi > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi + Registers.HL.hi) & 0xF) == 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi += Registers.HL.hi;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Add value pointed to by HL to A
void c_DMGCPU::OPCode0x86()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    uint8_t result = (Registers.AF.hi + MMU->ReadByte(Registers.HL.word));
    DbgOut(DBG_CPU, VERBOSE_0, "Adding (HL): 0x%x to A. A = 0x%x. Result = 0x%x", MMU->ReadByte(Registers.HL.word), Registers.AF.hi, result);

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((Registers.AF.hi + MMU->ReadByte(Registers.HL.word)) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if((Registers.AF.hi + MMU->ReadByte(Registers.HL.word)) > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi = result;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//ADD A to A
void c_DMGCPU::OPCode0x87()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    DbgOut(DBG_CPU, VERBOSE_2, "Adding A: 0x%x to A", Registers.AF.hi);

    if(Registers.AF.hi + Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi + Registers.AF.hi > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(Registers.AF.hi + Registers.AF.hi > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi += Registers.AF.hi;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//Add B + carry flag to A.
void c_DMGCPU::OPCode0x88()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    uint8_t carry = (FLAG_CARRY ? 1 : 0);
    uint8_t value = Registers.BC.hi + carry;
    uint8_t result = Registers.AF.hi + value;
    DbgOut(DBG_CPU, VERBOSE_2, "ADC A, B");

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((Registers.AF.hi + value) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if((Registers.AF.hi + value) > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Add C + carry flag to A.
void c_DMGCPU::OPCode0x89()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    uint8_t carry = (FLAG_CARRY ? 1 : 0);
    uint8_t value = Registers.BC.lo + carry;
    uint8_t result = Registers.AF.hi + value;
    DbgOut(DBG_CPU, VERBOSE_2, "ADC A, C");

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((Registers.AF.hi + value) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if((Registers.AF.hi + value) > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi = result;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//AND A with B
void c_DMGCPU::OPCode0xA0()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    SET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi &= Registers.BC.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "AND B");

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//AND A with C
void c_DMGCPU::OPCode0xA1()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    SET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi &= Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "AND C");

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//AND A with A
void c_DMGCPU::OPCode0xA7()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    SET_FLAG_BIT(HC_BIT);

    Registers.AF.hi &= Registers.AF.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "AND A. Result = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//XOR A with C
void c_DMGCPU::OPCode0xA9()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi ^= Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "XOR A. A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//XOR A with value pointed to by HL
void c_DMGCPU::OPCode0xAE()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi ^= MMU->ReadByte(Registers.HL.word);
    DbgOut(DBG_CPU, VERBOSE_2, "XOR A. (HL) = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//XOR A
void c_DMGCPU::OPCode0xAF()
{
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi ^= Registers.AF.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "XOR A. A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//OR A with B
void c_DMGCPU::OPCode0xB0()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi |= Registers.BC.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "OR A, B, A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//OR A with C
void c_DMGCPU::OPCode0xB1()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi |= Registers.BC.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "OR A, C, A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//OR A with E
void c_DMGCPU::OPCode0xB3()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi |= Registers.DE.lo;
    DbgOut(DBG_CPU, VERBOSE_2, "OR A, E. A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//OR A with A
void c_DMGCPU::OPCode0xB7()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi |= Registers.AF.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "OR A, A. A = 0x%x", Registers.AF.hi);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//CP (HL), Compare A against (HL)
void c_DMGCPU::OPCode0xBE()
{
    DbgOut(DBG_CPU, VERBOSE_2, "Comparing A to value at: 0x%x", Registers.HL.word);

    //Here we go...
    SET_FLAG_BIT(SUB_BIT); //Always set

    if(Registers.AF.hi == MMU->ReadByte(Registers.HL.word))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    //What the fuck hahaha
    //It's just checking if the 4 least significant bits of a - (hl) are greater than the 4 lsb's of A on its own.
    if(((Registers.AF.hi - MMU->ReadByte(Registers.HL.word)) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi < MMU->ReadByte(Registers.HL.word))
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;

}

//Return if zero flag is not set.
void c_DMGCPU::OPCode0xC0()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RET NZ");
    if(!FLAG_ZERO)
    {
        Registers.PC.word = MMU->ReadWord(Registers.SP.word);
        Registers.SP.word += 2;
        Clock.t = 20;
    }
    else {
        Registers.PC.word++;
        Clock.t = 8;
    }
    Clock.m = 1;
}

//POP BC from the Stack
void c_DMGCPU::OPCode0xC1()
{
    Registers.BC.word = MMU->ReadWord(Registers.SP.word);
    Registers.SP.word += 2;
    DbgOut(DBG_CPU, VERBOSE_2, "POP BC. New BC = 0x%x.", Registers.BC.word);
    Clock.m = 1;
    Clock.t = 12;
    Registers.PC.word++;
}

//Jump to immediate 16-bit address.
void c_DMGCPU::OPCode0xC3()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JP 0x%x", MMU->ReadWord(Registers.PC.word+1));
    Registers.PC.word =  MMU->ReadWord(Registers.PC.word+1);
    Clock.m = 3;
    Clock.t = 16;
}

//CALL 16-bit address if ZERO flag is not set
void c_DMGCPU::OPCode0xC4()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JP Z, (a16)");
    uint8_t flag = (FLAG_ZERO? 1 : 0);
    if(!flag)
    {
        Registers.PC.word = MMU->ReadWord(Registers.PC.word+1);
        Clock.t = 16;
    }
    else {
        Registers.PC.word += 3;
        Clock.t = 12;
    }
    Clock.m = 3;
}

//Push BC onto the stack
void c_DMGCPU::OPCode0xC5()
{
    DbgOut(DBG_CPU, VERBOSE_2, "PUSH BC. BC = 0x%x", Registers.BC.word);
    //Increment the stackpointer DOWNWARDS
    Registers.SP.word -= 2;
    //Push BC onto the stack according to where the stackpointer is
    MMU->WriteWord(Registers.SP.word, Registers.BC.word);

    Clock.m = 1;
    Clock.t = 16;
    Registers.PC.word++;
}

//Add immediate byte to A.
void c_DMGCPU::OPCode0xC6()
{
    UNSET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    uint8_t result = (Registers.AF.hi + MMU->ReadByte(Registers.PC.word+1));
    DbgOut(DBG_CPU, VERBOSE_2, "ADD A, d8");

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((Registers.AF.hi + MMU->ReadByte(Registers.PC.word+1)) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if((Registers.AF.hi + MMU->ReadByte(Registers.PC.word+1)) > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi = result;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Return if zero flag is set.
void c_DMGCPU::OPCode0xC8()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RET Z");
    if(FLAG_ZERO)
    {
        Registers.PC.word = MMU->ReadWord(Registers.SP.word);
        Registers.SP.word += 2;
        Clock.t = 20;
    }
    else {
        Registers.PC.word++;
        Clock.t = 8;
    }
    Clock.m = 1;
}

//Return if carry flag is not set.
void c_DMGCPU::OPCode0xD0()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RET NC");
    if(!FLAG_CARRY)
    {
        Registers.PC.word = MMU->ReadWord(Registers.SP.word);
        Registers.SP.word += 2;
        Clock.t = 20;
    }
    else {
        Registers.PC.word++;
        Clock.t = 8;
    }
    Clock.m = 1;
}

//POP DE from the Stack
void c_DMGCPU::OPCode0xD1()
{
    Registers.DE.word = MMU->ReadWord(Registers.SP.word);
    Registers.SP.word += 2;
    DbgOut(DBG_CPU, VERBOSE_2, "POP DE. New DE = 0x%x.", Registers.DE.word);
    Clock.m = 1;
    Clock.t = 12;
    Registers.PC.word++;
}

//Subtract immediate byte + Carry flag from A.
void c_DMGCPU::OPCode0xDE()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SBC A, d8");

    uint8_t carry = (FLAG_CARRY ? 1 : 0);

    uint8_t value = MMU->ReadByte(Registers.PC.word+1) + carry;

    if((Registers.AF.hi - value) < 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - value) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    if(Registers.AF.hi - value == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(SUB_BIT);

    Registers.AF.hi -= value;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//Load 8-bit value in A into memory pointed to by 0xFF00 + 8-bit immediate value.
void c_DMGCPU::OPCode0xE0()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD ($FF00 + d8), A. Addr = 0x%x. PC = 0x%x", (0xFF00 + MMU->ReadByte(Registers.PC.word + 1)), Registers.PC.word);
    MMU->WriteByte((0xFF00 + MMU->ReadByte(Registers.PC.word + 1)), Registers.AF.hi);
    Clock.m = 2;
    Clock.t = 12;
    Registers.PC.word += 2;
}

//POP HL from the stack
void c_DMGCPU::OPCode0xE1()
{
    Registers.HL.word = MMU->ReadWord(Registers.SP.word);
    Registers.SP.word += 2;
    DbgOut(DBG_CPU, VERBOSE_2, "POP HL HL = 0x%x", Registers.HL.word);
    Clock.m = 3;
    Clock.t = 12;
    Registers.PC.word++;
}

//Load 8-bit value in A into memory pointed to by 0xFF00 + C.
void c_DMGCPU::OPCode0xE2()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD ($FF00+C), A");
    MMU->WriteByte((0xFF00 + Registers.BC.lo), Registers.AF.hi);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word++;
}

//RST 28h. Push Present address to stack and jump to $0000 + 28
void c_DMGCPU::OPCode0xEF()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RST 28H");
    //Here we go...
    //Decrement Stack pointer
    Registers.SP.word -= 2;
    MMU->WriteWord(Registers.SP.word, (Registers.PC.word + 1)); //Write address of next instruction to stack.
    Clock.m = 4;
    Clock.t = 16;
    Registers.PC.word = 0x0028;
}

//Jump to immediate 16-bit address if zero flag isn't set.
void c_DMGCPU::OPCode0xC2()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JP NZ, (a16)");
    uint8_t flag = (FLAG_ZERO? 1 : 0);
    if(!flag)
    {
        Registers.PC.word = MMU->ReadWord(Registers.PC.word+1);
        Clock.t = 16;
    }
    else {
        Registers.PC.word += 3;
        Clock.t = 12;
    }
    Clock.m = 3;
}

//Return from function.
void c_DMGCPU::OPCode0xC9()
{
    Registers.PC.word = MMU->ReadWord(Registers.SP.word);
    Registers.SP.word += 2;
    DbgOut(DBG_CPU, VERBOSE_2, "RET. Return address = 0x%x", Registers.PC.word);
    Clock.m = 1;
    Clock.t = 16;
}

//Jump to immediate 16-bit address if zero flag is set.
void c_DMGCPU::OPCode0xCA()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JP Z, (a16)");
    uint8_t flag = (FLAG_ZERO ? 1 : 0);
    if(flag)
    {
        Registers.PC.word = MMU->ReadWord(Registers.PC.word+1);
        Clock.t = 16;
    }
    else {
        Registers.PC.word += 3;
        Clock.t = 12;
    }
    Clock.m = 3;
}

//Call function at immediate 16-bit address. Store address of next function in the stack + decrement stack pointer.
void c_DMGCPU::OPCode0xCD()
{
    //Write address of next instruction to the stack and decrement SP.
    Registers.SP.word -= 2;
    MMU->WriteWord(Registers.SP.word, Registers.PC.word + 3);
    DbgOut(DBG_CPU, VERBOSE_0, "CALL 0x%x. Return address = 0x%x", MMU->ReadWord(Registers.PC.word + 1), Registers.PC.word+3);
    //Set PC to address of function.
    Registers.PC.word = MMU->ReadWord(Registers.PC.word + 1);
    Clock.m = 3;
    Clock.t = 24;
}

//PUSH DE to the stack
void c_DMGCPU::OPCode0xD5()
{
    DbgOut(DBG_CPU, VERBOSE_2, "PUSH DE. DE = 0x%x", Registers.DE.word);
    //Increment the stackpointer DOWNWARDS
    Registers.SP.word -= 2;
    //Push BC onto the stack according to where the stackpointer is
    MMU->WriteWord(Registers.SP.word, Registers.DE.word);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Subtract immediate 8-bit value from A
void c_DMGCPU::OPCode0xD6()
{
    SET_FLAG_BIT(SUB_BIT); //We are performing an addition.

    uint8_t result = (Registers.AF.hi - MMU->ReadByte(Registers.PC.word+1));
    DbgOut(DBG_CPU, VERBOSE_2, "SUB, d8");

    if(result == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if((Registers.AF.hi - MMU->ReadByte(Registers.PC.word+1)) > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if((Registers.AF.hi - MMU->ReadByte(Registers.PC.word+1)) > 0xF)
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Registers.AF.hi = result;

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Return from interrupt handler and enable interrupts.
void c_DMGCPU::OPCode0xD9()
{
    Registers.PC.word = MMU->ReadWord(Registers.SP.word);
    DbgOut(DBG_CPU, VERBOSE_2, "RETI. Return address = 0x%x", Registers.PC.word);
    Registers.SP.word += 2;
    Clock.m = 1;
    Clock.t = 16;
    //Enable interrupts.
    IME = true;
}

//Jump to immediate 16-bit address if the carry flag is set.
void c_DMGCPU::OPCode0xDA()
{
    DbgOut(DBG_CPU, VERBOSE_2, "JP C, (a16)");
    uint8_t flag = (FLAG_CARRY? 1 : 0);
    if(flag)
    {
        Registers.PC.word = MMU->ReadWord(Registers.PC.word+1);
        Clock.t = 16;
    }
    else {
        Registers.PC.word += 3;
        Clock.t = 12;
    }
    Clock.m = 3;
}

//Push HL onto the stack
void c_DMGCPU::OPCode0xE5()
{
    DbgOut(DBG_CPU, VERBOSE_2, "PUSH HL. HL = 0x%x", Registers.HL.word);
    //Increment the stackpointer DOWNWARDS
    Registers.SP.word -= 2;
    //Push BC onto the stack according to where the stackpointer is
    MMU->WriteWord(Registers.SP.word, Registers.HL.word);

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//And A with 8-bit immediate value
void c_DMGCPU::OPCode0xE6()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    SET_FLAG_BIT(HC_BIT);
    Registers.AF.hi &= MMU->ReadByte(Registers.PC.word + 1);

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    DbgOut(DBG_CPU, VERBOSE_2, "AND A, d8 A = 0x%x", Registers.AF.hi);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2-byte instruction
}

//Add immediate 8-bit value to SP.
void c_DMGCPU::OPCode0xE8()
{
    DbgOut(DBG_CPU, VERBOSE_2, "Add SP, d8");
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    uint8_t operand = MMU->ReadByte(Registers.PC.word + 1);

    if((Registers.SP.word + operand) > 0xFFFF)
        SET_FLAG_BIT(CARRY_BIT);

    if((Registers.SP.word + operand) > 0xFF)
        SET_FLAG_BIT(HC_BIT);

    Clock.m = 2;
    Clock.t = 16;
    Registers.PC.word += 2;
}

//Jump to address contained in HL.
//Think this may have been breaking Tetris.
void c_DMGCPU::OPCode0xE9()
{
    DbgOut(DBG_CPU, VERBOSE_0, "LD PC, HL. Jumping to address 0x%x", Registers.HL.word);
    Registers.PC.word = Registers.HL.word;
    Clock.m = 1;
    Clock.t = 4;
}

//Write A to 16-bit immediate address.
void c_DMGCPU::OPCode0xEA()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD (a16), A");
    MMU->WriteByte(MMU->ReadWord(Registers.PC.word+1), Registers.AF.hi);

    Clock.m = 3;
    Clock.t = 16;
    Registers.PC.word += 3;
}

//Load memory location 0xFF00 + immediate 8-bit into A.
void c_DMGCPU::OPCode0xF0()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LDH A, (a8)");
    Registers.AF.hi = MMU->ReadByte(0xFF00 + MMU->ReadByte(Registers.PC.word+1));
    Clock.m = 2;
    Clock.t = 12;
    Registers.PC.word += 2;
}

//POP AF from the Stack
void c_DMGCPU::OPCode0xF1()
{
    Registers.AF.word = MMU->ReadWord(Registers.SP.word);
    Registers.SP.word += 2;
    DbgOut(DBG_CPU, VERBOSE_2, "POP AF. New AF = 0x%x.", Registers.AF.word);
    Clock.m = 1;
    Clock.t = 12;
    Registers.PC.word++;
}

//Disable interrupts at start of next cycle.
void c_DMGCPU::OPCode0xF3()
{
    DbgOut(DBG_CPU, VERBOSE_2, "DI");
    IME = false;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Push AF onto the stack.
void c_DMGCPU::OPCode0xF5()
{
    DbgOut(DBG_CPU, VERBOSE_2, "PUSH AF");

    //Increment the stackpointer DOWNWARDS
    Registers.SP.word -= 2;
    //Push AF onto the stack according to where the stackpointer is
    MMU->WriteWord(Registers.SP.word, Registers.AF.word);

    Clock.m = 1;
    Clock.t = 16;
    Registers.PC.word++;
}

//Load data at immediate address into A.
void c_DMGCPU::OPCode0xFA()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, (a16)");
    Registers.AF.hi = MMU->ReadByte(MMU->ReadWord(Registers.PC.word + 1));
    Clock.m = 3;
    Clock.t = 16;
    Registers.PC.word += 3;
}

//Enable Interrupts
void c_DMGCPU::OPCode0xFB()
{
    //Enable interrupts
    IME = true;
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

// CB opcodes.

//Rotate C left through the Carry Flag
void c_DMGCPU::OPCodeCB0x11()
{
    DbgOut(DBG_CPU, VERBOSE_2, "RL C");
    //Store carry flag.
    uint8_t carryi = FLAG_CARRY ? 1 : 0;
    uint8_t regi = Registers.BC.lo;

    //Unset flag bits
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);


    if(MSB(Registers.BC.lo) > 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    regi = (regi << 1) | carryi;

    Registers.BC.lo = regi;

    Clock.t = 2;
    Clock.m = 8;
    Registers.PC.word += 2;
}

//Shift B right into carry. Set MSB of B to 0
void c_DMGCPU::OPCodeCB0x38()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    uint8_t carry = MSB(Registers.BC.hi) ? 1 : 0;
    Registers.BC.hi >>= 1;
    UNSET_BIT(Registers.BC.hi, 7);

    if(Registers.BC.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(carry)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);
}

//Compare A with n.
void c_DMGCPU::OPCode0xFE()
{
    DbgOut(DBG_CPU, VERBOSE_2, "CP A, d8");
    uint8_t n = MMU->ReadByte(Registers.PC.word+1);

    //This whole instruction is basically a subtraction but we throw away the results, so set SUB bit.
    SET_FLAG_BIT(SUB_BIT);

    if(Registers.AF.hi == n)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    if(Registers.AF.hi < n)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);

    if(((Registers.AF.hi - n) & 0xF) > (Registers.AF.hi & 0xF))
        SET_FLAG_BIT(HC_BIT);
    else
        UNSET_FLAG_BIT(HC_BIT);

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Rotate A left
void c_DMGCPU::OPCodeCB0x17()
{
    //Store carry flag.
    uint8_t carryi = FLAG_CARRY ? 1 : 0;
    uint8_t regi = Registers.AF.hi;

    //Unset flag bits
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);


    if(MSB(Registers.AF.hi) > 0)
        SET_FLAG_BIT(CARRY_BIT);
    else
        UNSET_FLAG_BIT(CARRY_BIT);


    regi = (regi << 1) | carryi;

    DbgOut(DBG_CPU, VERBOSE_2, "RL A. Aorig = 0x%x. Anow= 0x%x carryi = %i.", Registers.AF.hi, regi, carryi);
    Registers.AF.hi = regi;

    Clock.t = 1;
    Clock.m = 4;
    //Needs to be changed.
    Registers.PC.word += 1;
}

//Shift E left into carry.
void c_DMGCPU::OPCodeCB0x23()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SLA E");
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    if(MSB(Registers.DE.lo))
    {
        SET_FLAG_BIT(CARRY_BIT);
    }
    else
    {
        UNSET_FLAG_BIT(CARRY_BIT);
    }

    Registers.DE.lo = Registers.DE.lo << 1;

    if(Registers.DE.lo == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 2;
    Clock.t = 8;

    Registers.PC.word += 2;
}

//Shift A left into carry.
void c_DMGCPU::OPCodeCB0x27()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SLA A");
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    if(MSB(Registers.AF.hi))
    {
        SET_FLAG_BIT(CARRY_BIT);
    }
    else
    {
        UNSET_FLAG_BIT(CARRY_BIT);
    }

    Registers.AF.hi = Registers.AF.hi << 1;

    if(Registers.AF.hi == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 2;
    Clock.t = 8;

    Registers.PC.word += 2;
}

//SWAP A
void c_DMGCPU::OPCodeCB0x37()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(CARRY_BIT);

    //Swap lower and higher nibbles of A
    uint8_t lower = Registers.AF.hi & 0xF;
    Registers.AF.hi = (Registers.AF.hi >> 4) | (lower << 4);
    DbgOut(DBG_CPU, VERBOSE_2, "SWAP A, A = 0x%x", Registers.AF.hi);

    if(Registers.AF.lo == 0)
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Test bit 0 of C register.
void c_DMGCPU::OPCodeCB0x41()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 0, C.");

    if(!(Registers.BC.lo & 0x01))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 0 of A register.
void c_DMGCPU::OPCodeCB0x47()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 0, A.");

    if(!(Registers.AF.hi & 0x01))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 4 of A.
void c_DMGCPU::OPCodeCB0x67()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 4, A.");

    if(!(Registers.AF.hi & 0x10))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 5 of A.
void c_DMGCPU::OPCodeCB0x6F()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 5, A.");

    if(!(Registers.AF.hi & 0x20))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 6 of A register.
void c_DMGCPU::OPCodeCB0x77()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 6, A.");

    if(!(Registers.AF.hi & 0x40))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 7(msb) of E register. Reset N. Set Z flag if bit is zero. Set H.
void c_DMGCPU::OPCodeCB0x7C()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 7, H. H = 0x%x", Registers.HL.hi);

    if(!(MSB(Registers.HL.hi)))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Test bit 7(msb) of A register.
void c_DMGCPU::OPCodeCB0x7F()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 7, A");

    if(!(MSB(Registers.AF.hi)))
        SET_FLAG_BIT(ZERO_BIT);
    else
        UNSET_FLAG_BIT(ZERO_BIT);

    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}

//Set bit 7 of memory location pointed to by HL.
void c_DMGCPU::OPCodeCB0xFE()
{
    DbgOut(DBG_CPU, VERBOSE_2, "SET 7, HL");

    uint8_t data = MMU->ReadByte(Registers.HL.word);

    data |= 0x80; //Set bit 7 to on.

    Clock.m = 2;
    Clock.t = 16;
    Registers.PC.word += 2;
}
