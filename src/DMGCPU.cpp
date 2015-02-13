#include "DMGCPU.h"
#include "DMG_opcodes.h"
#include "debug.h"

c_DMGCPU::c_DMGCPU(c_MMU* pMMU)
{
    InitOpcodeTables();
    MMU = pMMU;
}

c_DMGCPU::~c_DMGCPU()
{

}

//Run one instruction.
void c_DMGCPU::Tick()
{
    if(running)
    {
        switch(MMU->ReadByte(Registers.PC.word))
        {
            case 0xCB:
                //0xCB special opcode. Look up in separate opcode table.
                DbgOut(DBG_CPU, VERBOSE_2, "0xCB opcode: 0x%x", MMU->ReadByte(Registers.PC.word + 1));
                (this->*OPCodesCB[MMU->ReadByte(Registers.PC.word + 1)])();
            break;

            default:
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
    //Clear opcode tables with NULL (easy to check for invalid operation / unimplemented opcode)
    memset(c_DMGCPU::OPCodes, NULL, 0xFF);
    memset(c_DMGCPU::OPCodesCB, NULL, 0xFF);

    //Fill our opcode table
    OPCodes[0x00] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x01] = &c_DMGCPU::OPCode0x01;
    OPCodes[0x02] = &c_DMGCPU::OPCode0x02;
    OPCodes[0x02] = &c_DMGCPU::OPCode0x03;
    OPCodes[0x03] = &c_DMGCPU::OPCode0x04;
    OPCodes[0x04] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x05] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x06] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x07] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x08] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x09] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x0A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x0B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x0C] = &c_DMGCPU::OPCode0x0C;
    OPCodes[0x0D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x0E] = &c_DMGCPU::OPCode0x0E;
    OPCodes[0x0F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x10] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x11] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x12] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x13] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x14] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x15] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x16] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x17] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x18] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x19] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x1F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x20] = &c_DMGCPU::OPCode0x20;
    OPCodes[0x21] = &c_DMGCPU::OPCode0x21;
    OPCodes[0x22] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x23] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x24] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x25] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x26] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x27] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x28] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x29] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x2F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x30] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x31] = &c_DMGCPU::OPCode0x31;
    OPCodes[0x32] = &c_DMGCPU::OPCode0x32;
    OPCodes[0x33] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x34] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x35] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x36] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x37] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x38] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x39] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x3A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x3B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x3C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x3D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x3E] = &c_DMGCPU::OPCode0x3E;
    OPCodes[0x3F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x40] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x41] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x42] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x43] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x44] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x45] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x46] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x47] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x48] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x49] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x4F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x50] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x51] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x52] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x53] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x54] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x55] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x56] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x57] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x58] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x59] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x5F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x60] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x61] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x62] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x63] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x64] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x65] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x66] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x67] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x68] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x69] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x6F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x70] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x71] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x72] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x73] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x74] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x75] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x76] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x77] = &c_DMGCPU::OPCode0x77;
    OPCodes[0x78] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x79] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x7F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x80] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x81] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x82] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x83] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x84] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x85] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x86] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x87] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x88] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x89] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x8F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x90] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x91] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x92] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x93] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x94] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x95] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x96] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x97] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x98] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x99] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9A] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9B] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9C] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9D] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9E] = &c_DMGCPU::OPCode0x00;
    OPCodes[0x9F] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA0] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA2] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xA9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAA] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAB] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAD] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xAF] = &c_DMGCPU::OPCode0xAF;
    OPCodes[0xB0] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB2] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xB9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBA] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBB] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBD] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xBF] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC0] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC2] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xC9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xCA] = &c_DMGCPU::OPCode0x00;
    //Function to read a CB opcode, not an actual opcode in itself
    //There is a switch to check if it's a CB opcode now. I think
    //we're better making a separate opcode table for them.
    OPCodes[0xCC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xCD] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xCE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xCF] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD0] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD2] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xD9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDA] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDB] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDD] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xDF] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE0] = &c_DMGCPU::OPCode0xE0;
    OPCodes[0xE1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE2] = &c_DMGCPU::OPCode0xE2;
    OPCodes[0xE3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xE9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xEA] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xEB] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xEC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xED] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xEE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xEF] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF0] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF1] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF2] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF3] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF4] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF5] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF6] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF7] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF8] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xF9] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFA] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFB] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFC] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFD] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFE] = &c_DMGCPU::OPCode0x00;
    OPCodes[0xFF] = &c_DMGCPU::OPCode0x00;

    //CB opcode table. Make sure to update tick function when opcodes are implemented.
    OPCodesCB[0x7C] = &c_DMGCPU::OPCodeCB0x7C;
}

//NOP Instruction - Temporarily using as unknown opcode. BIOS doesn't use NOP.
void c_DMGCPU::OPCode0x00()
{
    DbgOut(DBG_CPU, VERBOSE_0, "-------------------------------------------------");
    DbgOut(DBG_CPU, VERBOSE_0, "Unknown opcode/NOP: 0x%x. PC: 0x%x", MMU->ReadByte(Registers.PC.word), Registers.PC.word);
    running = false; //Halt CPU.
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

    //Registers.PC.word++;
    //Clock.m = 1;
    //Clock.t = 4;
}

//Load immediate 16-bit value into BC
void c_DMGCPU::OPCode0x01()
{
    Registers.BC.word = MMU->ReadWord(Registers.PC.word + 1);
    Clock.m = 3;
    Clock.t = 12;
    Registers.PC.word += 3;
}

//Load A into the address stored in BC
void c_DMGCPU::OPCode0x02()
{
    MMU->WriteByte(Registers.BC.word, Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word += 1;
}

//Increment BC
void c_DMGCPU::OPCode0x03()
{
    Registers.BC.word++;
    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word += 1;
}

//Increment B
void c_DMGCPU::OPCode0x04()
{
    UNSET_FLAG_BIT(SUB_BIT); //Reset the subtraction bit in FLAGS

    if(Registers.BC.hi++ > 0x0F) //Pretty sure this is how it's done, not 100%
        SET_FLAG_BIT(HC_BIT);

    if(!(Registers.BC.hi++ & 0xFF))
        SET_FLAG_BIT(ZERO_BIT);

    Registers.BC.hi++; //Actually do the checks

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word += 1;
}

//Decrement B
void c_DMGCPU::OPCode0x05()
{
    SET_FLAG_BIT(SUB_BIT);

    if(Registers.BC.hi-- > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);

    if(Registers.BC.hi-- > 0x0F)
        SET_FLAG_BIT(HC_BIT);

    Registers.BC.word--;

    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load 9-bit immediate value into B
void c_DMGCPU::OPCode0x06()
{
    Registers.BC.hi = MMU->ReadByte(Registers.PC.hi + 1);
    Clock.m = 3;
    Clock.t = 8;
    Registers.PC.word += 2;
}

//Rotate A left through Carry bit (Bit 7 to carry and Carry bit)
void c_DMGCPU::OPCode0x07()
{
    UNSET_FLAG_BIT(SUB_BIT);
    UNSET_FLAG_BIT(ZERO_BIT);
    UNSET_FLAG_BIT(HC_BIT);

    if(MSB(Registers.AF.hi))
        SET_FLAG_BIT(CARRY_BIT);

    Registers.AF.hi <<= 1;
    Registers.AF.hi |= Registers.AF.lo & CARRY_BIT;

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

    if(Registers.HL.word + Registers.BC.word > 0xFF)
        SET_FLAG_BIT(CARRY_BIT);

    if(Registers.HL.word + Registers.BC.word > 0x0F)
        SET_FLAG_BIT(HC_BIT);

    Registers.HL.word += Registers.BC.word;

    Clock.m = 1;
    Clock.t = 8;
    Registers.PC.word++;
}

//Load value at address in stored in BC into A
void c_DMGCPU::OPCode0x0A()
{
    Registers.AF.hi = MMU->ReadByte(Registers.BC.word);

    Clock.m = 1;
    Clock.t = 4;
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
    UNSET_FLAG_BIT(SUB_BIT);

    if(!(Registers.BC.lo++ & 0xFF))
        SET_FLAG_BIT(ZERO_BIT);

    if(Registers.BC.lo++ > 0x0F)
        SET_FLAG_BIT(HC_BIT);

    Registers.BC.lo++;

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

//Load immediate 8-bit value into A.
void c_DMGCPU::OPCode0x3E()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD A, d8");
    Registers.AF.hi = MMU->ReadByte(Registers.PC.word + 1);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2;
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

//XOR A
void c_DMGCPU::OPCode0xAF()
{
    Registers.AF.hi ^= Registers.AF.hi;
    DbgOut(DBG_CPU, VERBOSE_2, "XOR A. A = 0x%x", Registers.AF.hi);
    Clock.m = 1;
    Clock.t = 4;
    Registers.PC.word++;
}

//Load 8-bit value in A into memory pointed to by 0xFF00 + 8-bit immediate value.
void c_DMGCPU::OPCode0xE0()
{
    DbgOut(DBG_CPU, VERBOSE_2, "LD ($FF00 + d8), A");
    MMU->WriteByte((0xFF00 + MMU->ReadByte(Registers.PC.word + 1)), Registers.AF.hi);
    Clock.m = 2;
    Clock.t = 12;
    Registers.PC.word += 2;
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

// CB opcodes.

//Test bit 7(msb) of E register. Reset N. Set Z flag if bit is zero. Set H.
void c_DMGCPU::OPCodeCB0x7C()
{
    DbgOut(DBG_CPU, VERBOSE_2,  "BIT 7, H. H = 0x%x", Registers.HL.hi);
    if(!(Registers.HL.hi & 0x80))
    {
        SET_FLAG_BIT(ZERO_BIT);
    }
    else {
        UNSET_FLAG_BIT(ZERO_BIT);
    }
    SET_FLAG_BIT(HC_BIT);
    UNSET_FLAG_BIT(SUB_BIT);
    Clock.m = 2;
    Clock.t = 8;
    Registers.PC.word += 2; //2 bytes, as this is a 2-byte opcode.
}
