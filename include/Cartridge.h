#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

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
#define ROM+MBC3+TIMER+BATT     0x0F
#define ROM+MBC3+TIMER+RAM+BATT 0x10
#define ROM+MBC3                0x11
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
#define HUDSON_HUC-1            0xFF

//Just storing info, so let's just use a struct instead of a class
typedef struct
{
    char* title; //Game title
    uint8_t gbtype; //Type of gameboy; 0x80 = CGB, 0x00 = Other/Not CGB
    uint8_t gbsgb; //Super Gameboy functionality; 0x00 = Gameboy, 0x03 = SGB
    uint8_t cart_type; //Cartridge type
    uint8_t rom_size; //Size of the ROM/How many banks.
    uint8_t ram_size; //Size of RAM/How many banks.
    uint8_t region; //Region; 0 = Japanese, 1 = Non-Japanese
    uint8_t licensee; //Lincensee code; 0x33 = Check 0x144, 0x79 = Accolade, 0xA4 = Konami (SGB Won't work if != 0x33)
    uint8_t rom_ver; //MASK ROM Version
    uint16_t checksum; //Checksum (Higher Byte first)
}Cartridge;

#endif // CARTRIDGE_H
