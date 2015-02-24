#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

#define ROM_ONLY            0x00
#define MBC1                0x01
#define MBC1_RAM            0x02
#define MBC1_RAM_BATT       0x03
#define MBC2                0x05
#define MBC2_BATT           0x06
#define RAM                 0x08
#define RAM_BATT            0x09
#define MMM01               0x0B
#define MMM01_SRAM          0x0C
#define MMM01_SRAM_BATT     0x0D
#define MBC3_TIMER_BATT     0x0F
#define MBC3_TIMER_RAM_BATT 0x10
#define MBC3                0x11
#define MBC3_RAM            0x12
#define MBC3_RAM_BATT       0x13
#define MBC5                0x19
#define MBC5_BATT           0x1A
#define MBC5_RAM_BATT       0x1B
#define MBC5_RUMBLE         0x1C
#define MBC5_RUMBLE_SRAM    0x1D
#define MBC5_RUMB_SRAM_BATT 0x1E
#define POCKET_CAMERA       0x1F
#define BANDAI_TAMA5        0xFD
#define HUDSON_HUC_3        0xFE
#define HUDSON_HUC_1        0xFF

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
    uint8_t ver; //MASK ROM Version
    uint16_t checksum; //Checksum (Higher Byte first)
}Cartridge;

#endif // CARTRIDGE_H
