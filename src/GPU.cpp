#include "GPU.h"
#include "debug.h"
#include "canvas.h"

c_GPU::c_GPU(c_Canvas* cnv)
{
    DbgOut(DBG_VID, VERBOSE_0, "Initialising GPU...");
    canvas = cnv;
    stateclock = 0;
    state = 0;
    line = 0;
    memset(&vram, 0, 0x2000);
    memset(&tileset, 0, 384*8*8);
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
            scy = data;
        break;

        //SCX.
        case 0xFF43:
            scx = data;
        break;

        default:
            DbgOut(DBG_VID, VERBOSE_0, "Unimplemented or read only GPU register: 0x%x", addr);
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

        //Set Colour palette data. Assuming we should put these values into pixel_t struct ()
        //These are what defines what shades of gray to use.
        case 0xFF47:
            return 0x00;
        break;

        //Line register.
        case 0xFF44:
            return line;
        break;

        default:
            DbgOut(DBG_VID, VERBOSE_0, "Unimplemented GPU register: 0x%x", addr);
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

    DbgOut(DBG_CPU, VERBOSE_1, "Updating tile: %i. Line: %i", tileindex, y);

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
    uint16_t addr = 0x9900; //Start of the tilemap for the BIOS.
    uint16_t addrtrans = addr & 0x1FFE;
    uint8_t x;
    uint16_t tindex;

    for(x = 0; x <160; x++)
    {
        if((line/8) % 2) //Check if tile is even or odd.
        {
            tindex = vram[addrtrans + (x/8) + 0x20];
        }
        else {
            tindex = vram[addrtrans + (x/8)];
        }
        //DbgOut(DBG_VID, VERBOSE_0, "Drawing tile: %i.", vram[addrtrans + (x/8)]);
        if(tileset[tindex][line%8][x%8] > 0)
        {
            canvas->PutPixel(x, line, 0, 0, 0);
        }
        else {
            canvas->PutPixel(x, line, 255, 255, 255);
        }
    }

}

void c_GPU::Tick(uint32_t clock)
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
                    state = STATE_VBLANK;
                    //Draw framebuffer to the screen here.
                    canvas->Refresh();
                }
                else {
                    state = STATE_OAM_READ;
                }
            }
        break;

        case STATE_VBLANK:
            //DbgOut(DBG_VID, VERBOSE_1, "STATE = VBLANK");
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
