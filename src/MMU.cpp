#include "MMU.h"
#include "GPU.h"

c_MMU::c_MMU(c_GPU* pGPU)
{
    GPU = pGPU;
}

c_MMU::~c_MMU()
{
}

uint8_t c_MMU::ReadByte(uint16_t addr)
{
    if(biosmapped && ((addr>=0x00) && (addr<=0xFF)))
        return bios[addr];
    else
    {
        switch((addr & 0xF000) >> 12)
        {
            // ROM 0
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                return rom[0][addr];
            break;

            // ROM 1 (unbanked)
            case 0x4:
            case 0x5:
            case 0x6:
            case 0x7:
                return rom[activerombank][addr - 0x4000];
            break;

            // Video RAM
            case 0x8:
            case 0x9:
                return vram[addr - 0x8000];
            break;

            // External RAM
            case 0xA:
            case 0xB:
                return eram[addr - 0xA000];
            break;

            // Working RAM
            case 0xC:
            case 0xD:
                return wram[addr - 0xC000];
            break;

            // Working RAM shadow
            case 0xE:
                return wram[addr - 0xE000];
            break;

            // Working RAM shadow, IO, Zero Page RAM
            case 0xF:
                switch(addr & 0x0F00)
                {
                    // Working RAM shadow.
                    case 0x000: case 0x100: case 0x200: case 0x300:
                    case 0x400: case 0x500: case 0x600: case 0x700:
                    case 0x800: case 0x900: case 0xA00: case 0xB00:
                    case 0xC00: case 0xD00:
                        return wram[addr & 0x1FFF];
                    break;

                    // Object attribute memory.
                    case 0xE00:
                        if(addr < 0xFEA0)
                            return oam[addr & 0xFF];
                        else
                            return 0;
                    break;

                    case 0xF00:
                        if(addr == 0xFFFF)
                            return intenable;
                        else if(addr >= 0xFF80)
                            return zram[addr & 0x7F];
                        else
                        {
                            //IO
                            switch(addr & 0x00F0)
                            {

                                case 0x00:
                                    if(addr == 0xFF0F)
                                        return intflags;
                                break;

                                //GPU
                                case 0x40: case 0x50: case 0x60: case 0x70:
                                    return GPU->ReadReg(addr);
                                break;

                            }
                        }
                    break;

                }
            break;
        }
    }
}

uint16_t c_MMU::ReadWord(uint16_t addr)
{
    return (ReadByte(addr) | (ReadByte(addr+1) << 8));
}

void c_MMU::WriteByte(uint16_t addr, uint8_t data)
{
    if(biosmapped && ((addr>=0x00) & (addr<=0xFF)))
        return; //Read only memory. Can't write here.

    else
    {
        switch((addr & 0xF000) >> 12)
        {
            // ROM 0
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                return; // This is read only memory. We can't write here.
            break;

            // ROM 1 (unbanked)
            case 0x4:
            case 0x5:
            case 0x6:
            case 0x7:
                return; // This is read only memory. Do not write.
            break;

            // Video RAM
            case 0x8:
            case 0x9:
                //Forward write request on to GPU.
                GPU->WriteByte(addr, data);
            break;

            // External RAM
            case 0xA:
            case 0xB:
                eram[addr - 0xA000] = data;
            break;

            // Working RAM
            case 0xC:
            case 0xD:
                wram[addr - 0xC000] = data;
            break;

            // Working RAM shadow
            case 0xE:
                wram[addr - 0xE000] = data;
            break;

            // Working RAM shadow, IO, Zero Page RAM
            case 0xF:
                switch(addr & 0x0F00)
                {
                    // Working RAM shadow.
                    case 0x000: case 0x100: case 0x200: case 0x300:
                    case 0x400: case 0x500: case 0x600: case 0x700:
                    case 0x800: case 0x900: case 0xA00: case 0xB00:
                    case 0xC00: case 0xD00:
                        wram[addr & 0x1FFF] = data;
                    break;

                    // Object attribute memory.
                    case 0xE00:
                        if(addr < 0xFEA0)
                            oam[addr & 0xFF] = data;
                        else
                            return;
                    break;

                    case 0xF00:
                        if(addr == 0xFFFF)
                        {
                            intenable = data;
                        }
                        else if(addr >= 0xFF80)
                            zram[addr & 0x7F] = data;
                        else
                        {
                            //IO.
                            switch(addr & 0x00F0)
                            {
                                case 0x00:
                                    switch(addr & 0x000F)
                                    {
                                        //Interrupt flags.
                                        case 0x0F:
                                            intflags = data;
                                        break;
                                    }
                                break;
                                //GPU
                                case 0x40:
                                case 0x50:
                                case 0x60:
                                case 0x70:
                                    if((addr == 0xFF50) && (data == 0x01))
                                    {
                                        MapBIOS(false); //BIOS has finished running. Map bank 0 to lower memory instead.
                                    }
                                    else {
                                        //gpu->WriteByte(addr, data);
                                        GPU->WriteReg(addr, data);
                                    }
                                break;
                            }
                        }
                    break;

                }
            break;
        }
    }
}

void c_MMU::WriteWord(uint16_t addr, uint16_t data)
{
    WriteByte(addr, data & 255);
    WriteByte(addr+1, data >> 8);
}

void c_MMU::MapBIOS(bool ismap)
{
    DbgOut(DBG_MMU, VERBOSE_0, "BIOS mapped = %d", ismap);
    biosmapped = ismap;
}

void c_MMU::LoadBIOS(const char *fname)
{
    uint8_t *buffer;
    std::ifstream biosfile(fname, std::ios::in | std::ios::binary | std::ios::ate);
    size_t filesize, bytesread;

    if(biosfile.is_open())
    {
        // Read from disk BIOS to temporary buffer.
        DbgOut(DBG_MMU, VERBOSE_0,  "BIOS file found. Loading...");
        filesize = biosfile.tellg();
        DbgOut(DBG_MMU, VERBOSE_0,  "Reading %i bytes.", filesize);
        buffer = new uint8_t[filesize];

        biosfile.seekg(0, std::ios::beg);
        biosfile.read((char*)buffer, filesize);
        bytesread = biosfile.gcount();
        DbgOut(DBG_MMU, VERBOSE_0,  "Bytes read: %i.", bytesread);
        biosfile.close();

        int i;
        for(i=0; i<bytesread; i++)
            bios[i] = buffer[i];

        delete buffer;
    }
    else
    {
        DbgOut(DBG_MMU, VERBOSE_0,  "Error loading BIOS file!");
    }
}

void c_MMU::LoadROM(const char *fname)
{
    uint8_t *buffer;
    std::ifstream romfile(fname, std::ios::in | std::ios::binary | std::ios::ate);
    size_t filesize, bytesread;

    if(romfile.is_open())
    {
        // Read from disk BIOS to temporary buffer.
        DbgOut(DBG_MMU, VERBOSE_0,  "ROM file found. Loading...");
        filesize = romfile.tellg();
        DbgOut(DBG_MMU, VERBOSE_0,  "Reading %i bytes.", filesize);
        buffer = new uint8_t[filesize];

        romfile.seekg(0, std::ios::beg);
        romfile.read((char*)buffer, filesize);
        bytesread = romfile.gcount();
        DbgOut(DBG_MMU, VERBOSE_0,  "Bytes read: %i.", bytesread);
        romfile.close();

        uint16_t i;
        uint8_t rombank;
        // Store in Gameboy memory.
        for(rombank=0; rombank<0x02; rombank++)
        {
            for(i=0; i<0x4000; i++)
            {
                rom[rombank][i] = buffer[i + (rombank*0x4000)];
            }
        }

        delete buffer;
    }
    else
    {
        DbgOut(DBG_MMU, VERBOSE_0,  "Error loading ROM file!");
    }
}

void c_MMU::faulthandler(uint16_t faultaddr)
{

}
