#include "GPU.h"
#include "debug.h"

c_GPU::c_GPU()
{
    DbgOut(DBG_VID, VERBOSE_0, "Initialising GPU...");
    stateclock = 0;
    state = 0;
    line = 0;
}

c_GPU::~c_GPU()
{

}

void c_GPU::Tick(uint32_t clock)
{

    stateclock += clock;

    switch(state)
    {
        case STATE_OAM_READ:
            DbgOut(DBG_VID, VERBOSE_1, "STATE = OAM_READ");
            if(stateclock >= 80)
            {
                stateclock = 0;
                state = STATE_VRAM_READ;
            }
        break;

        case STATE_VRAM_READ:
            DbgOut(DBG_VID, VERBOSE_1, "STATE = VRAM_READ");
            if(stateclock >= 172)
            {
                stateclock = 0;
                state = STATE_HBLANK;

                //Draw a scanline to the framebuffer here.
            }
        break;

        case STATE_HBLANK:
            DbgOut(DBG_VID, VERBOSE_1, "STATE = HBLANK");
            if(stateclock >= 204)
            {
                stateclock = 0;
                line++;

                if(line == 143)
                {
                    //Finished drawing the last time. State should be VBLANK now.
                    state = STATE_VBLANK;
                    //Draw framebuffer to the screen here.
                }
                else {
                    state = STATE_OAM_READ;
                }
            }
        break;

        case STATE_VBLANK:
            DbgOut(DBG_VID, VERBOSE_1, "STATE = VBLANK");
            //MMU->WriteByte(0xFF0F, 0x01); //Set VBLANK interrupt flag.
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
    }
}
