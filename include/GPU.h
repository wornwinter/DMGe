#ifndef GPU_H
#define GPU_H

#include <cstring>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "canvas.h"
#include "debug.h"
#include "macro.h"


//GPU state definitions.
#define STATE_OAM_READ 2
#define STATE_VRAM_READ 3
#define STATE_HBLANK 0
#define STATE_VBLANK 1

class c_MMU;

class c_GPU {

    private:
        uint32_t stateclock;
        uint8_t line;
        //Three dimensional array for tiles. Tile index, y, x.
        uint8_t tileset[384][8][8];
        uint8_t vram[0x2000];
        uint8_t scx, scy; //Scroll registers.
        uint8_t wndx, wndy; //Window registers
        uint8_t bgbuffer[256][256];
        t_Pixel paletteref[4];
        uint8_t palette[4];
        c_Canvas* canvas;
        void RenderScanline();
        void RenderScreen();
        void ClearScreen();
        bool vblankstart;
        void DebugDraw();
        bool lockvram = false;

    public:
        c_GPU(c_Canvas *cnv);
        ~c_GPU();
        void WriteByte(uint16_t addr, uint8_t data);
        void WriteReg(uint16_t addr, uint8_t data);
        uint8_t ReadReg(uint16_t addr);
        void UpdateTile(uint16_t addr, uint8_t data);

        void Tick(uint32_t clock, c_MMU* MMU);
        uint8_t state;

};
#endif // GPU_H
