#include "GPU.h"
#include "debug.h"
#include "canvas.h"
#include "MMU.h"

c_GPU::c_GPU(c_Canvas* cnv)
{
    DbgOut(DBG_VID, VERBOSE_0, "Initialising GPU...");
    canvas = cnv;
    stateclock = 0;
    state = 0;
    line = 0;
    memset(&vram, 0, 0x2000);
    memset(&tileset, 0, 384*8*8);
    paletteref[0] = {156, 189, 15};
    paletteref[1] = {140, 173, 15};
    paletteref[2] = {48, 98, 48};
    paletteref[3] = {15, 56, 15};
    vblankstart = true;
}

c_GPU::~c_GPU()
{

}

void c_GPU::WriteReg(uint16_t addr, uint8_t data)
{
    switch(addr)
    {
        //LCD control register.
        case 0xFF40:
        break;

        //SCY.
        case 0xFF42:
            DbgOut(DBG_VID, VERBOSE_2, "Writing SCY: 0x%x", data);
            scy = data;
        break;

        //SCX.
        case 0xFF43:
            DbgOut(DBG_VID, VERBOSE_2, "Writing SCX: 0x%x", data);
            scx = data;
        break;

        //Set Colour palette data. Assuming we should put these values into pixel_t struct ()
        //These are what defines what shades of gray to use.
        case 0xFF47:
            DbgOut(DBG_VID, VERBOSE_1, "Writing palette: 0x%x", data);
            palette[3] = (data & 0xC0) >> 6;
            palette[2] = (data & 0x30) >> 4;
            palette[1] = (data & 0x0C) >> 2;
            palette[0] = (data & 0x03);
        break;

        //WY
        case 0xFF4A:
            DbgOut(DBG_VID, VERBOSE_2, "Writing WNDY: 0x%x", data);
            wndy = data;
        break;

        //WX
        case 0xFF4B:
            DbgOut(DBG_VID, VERBOSE_2, "Writing WNDX: 0x%x", data);
            wndx = data;
        break;

        default:
            DbgOut(DBG_VID, VERBOSE_1, "Unimplemented or read only GPU register: 0x%x", addr);
        break;
    }
}

uint8_t c_GPU::ReadReg(uint16_t addr)
{
    switch(addr)
    {
        //LCD control register.
        case 0xFF40:
            return 0x00;
        break;

        //SCY.
        case 0xFF42:
            return scy;
        break;

        //SCX.
        case 0xFF43:
            return scx;
        break;


        //Line register.
        case 0xFF44:
            return line;
        break;

        //Window Y register
        case 0xFF4A:
            return wndy;
        break;

        //Window X Register
        case 0xFF4B:
            return wndx;
        break;

        default:
            DbgOut(DBG_VID, VERBOSE_1, "Unimplemented GPU register: 0x%x", addr);
            return 0x00;
        break;
    }
}

void c_GPU::WriteByte(uint16_t addr, uint8_t data)
{
    //GameBoy code has requested to write to vram.
    //Translate address.
    uint16_t addrtrans = addr - 0x8000;
    vram[addrtrans] = data;

    UpdateTile(addr, data);
    DbgOut(DBG_VID, VERBOSE_1, "GPU write. Addr: 0x%x. Translated: 0x%x. Data: 0x%x", addr, addrtrans, data);
}

void c_GPU::UpdateTile(uint16_t addr, uint8_t data)
{
    uint16_t addrtrans = addr & 0x1FFE;
    uint16_t tileindex = (addrtrans >> 4);
    uint8_t y = (addrtrans >> 1) & 7;

    DbgOut(DBG_CPU, VERBOSE_0, "Updating tile: %i. Line: %i", tileindex, y);

    uint8_t sx, x;

    for(x = 0; x < 8; x++)
    {
        sx = 1 << (7-x);

        tileset[tileindex][y][x] = ((vram[addrtrans] & sx) ? 1 : 0)
        + ((vram[addrtrans+1] & sx) ? 2 : 0);
    }
}

//Render a to the display.
void c_GPU::RenderScanline()
{
    //Tile map #0 - BIOS uses this, so it's a good place to start.
    uint16_t tilemapbase = 0x1800;
    uint16_t offsetbase = tilemapbase + ((((line+scy)&255)>>3)<<5);
    uint8_t x, y, tindex;

    y = (line + scy) & 7;

    for(x = 0; x < 160; x++)
    {
        tindex = vram[offsetbase + (x/8)];
        canvas->PutPixel(x, line, paletteref[palette[tileset[tindex][y][x % 8]]]);
    }
}

void c_GPU::ClearScreen()
{
    int x, y;
    for(x = 0; x < 160; x++)
    {
        for(y = 0; y < 144; y++)
        {
            //canvas->PutPixel(x, y, 255, 255, 255);
        }
    }
}

void c_GPU::Tick(uint32_t clock, c_MMU* MMU)
{

    stateclock += clock;
    //DbgOut(DBG_VID, VERBOSE_1, "Tick.");
    switch(state)
    {
        case STATE_OAM_READ:
            //DbgOut(DBG_VID, VERBOSE_1, "STATE = OAM_READ");
            if(stateclock >= 80)
            {
                stateclock = 0;
                state = STATE_VRAM_READ;
            }
        break;

        case STATE_VRAM_READ:
            //DbgOut(DBG_VID, VERBOSE_1, "STATE = VRAM_READ");
            if(stateclock >= 172)
            {
                stateclock = 0;
                state = STATE_HBLANK;

                //Draw a scanline to the framebuffer here.
                RenderScanline();

            }
        break;

        case STATE_HBLANK:
            //DbgOut(DBG_VID, VERBOSE_1, "STATE = HBLANK");
            if(stateclock >= 204)
            {
                stateclock = 0;
                line++;

                if(line == 143)
                {
                    //Finished drawing the last time. State should be VBLANK now.
                    vblankstart = true;
                    state = STATE_VBLANK;
                    //Draw framebuffer to the screen here.
                    //ClearScreen();
                    canvas->Refresh();
                }
                else {
                    state = STATE_OAM_READ;
                }
            }
        break;

        case STATE_VBLANK:
            //DbgOut(DBG_VID, VERBOSE_1, "STATE = VBLANK");
            if(vblankstart)
            {
                //Set vblank interrupt flag.
                MMU->intflags |= 0x1;
                vblankstart = false;
            }
            else {
                //Disable vblank flag.
                MMU->intflags &= 0x1E;
            }
            if(stateclock >= 456)
            {
                stateclock = 0;
                line++;

                if(line > 153)
                {
                    state = STATE_OAM_READ;
                    line = 0;
                }
            }
        break;

        default:
            DbgOut(DBG_VID, VERBOSE_0, "GPU state error. State: %i. Resetting state.", state);
            state = 0;
            break;
    }
}
