#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#define ROM_ONLY                0x00
#define ROM+MBC1                0x01
#define ROM+MBC1+RAM            0x02
#define ROM+MBC1+RAM+BATT       0x03
#define ROM+MBC2                0x05
#define ROM+MBC2+BATT           0x06
#define ROM+RAM                 0x08
#define ROM+RAM+BATT            0x09
#define ROM+MMM01               0x0B
#define ROM+MMM01+SRAM          0x0C
#define ROM+MMM01+SRAM+BATT     0x0D
#define ROM+MBC3+RAM            0x12
#define ROM+MBC3+RAM+BATT       0x13
#define ROM+MBC5                0x19
#define ROM+MBC5+BATT           0x1A
#define ROM+MBC5+RAM+BATT       0x1B
#define ROM+MBC5+RUMBLE         0x1C
#define ROM+MBC5+RUMBLE+SRAM    0x1D
#define ROM+MBC5+RUMB+SRAM+BATT 0x1E
#define POCKET_CAMERA           0x1F
#define BANDAI_TAMA5            0xFD
#define HUDSON_HUC-3            0xFE


class c_Cartridge
{

};



#endif // CARTRIDGE_H
