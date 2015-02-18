#include "canvas.h"
#include "debug.h"

c_Canvas::c_Canvas(wxWindow *parent, const wxWindowID id,
            const wxPoint &pos, const wxSize &sz,
            long style, const wxString &name)
            : wxGLCanvas(parent, (wxGLCanvas*)0, id, pos, sz, style, name),
            init(false)
{
    GenTestPattern();
}

c_Canvas::~c_Canvas(void)
{

}

BEGIN_EVENT_TABLE(c_Canvas, wxGLCanvas)
    EVT_PAINT(c_Canvas::OnPaint)
END_EVENT_TABLE()

void c_Canvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    SetCurrent();
    if(!init)
    {
        InitGL();
        init = true;
    }

    //This needs a LOT of work. Going to try drawing using textured tris/quads instead.
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, &pixels);
	SwapBuffers();
	event.Skip();
	//DbgOut(DBG_APP, VERBOSE_0, "Paint");
}

//Generates a greyscale XOR pattern.
void c_Canvas::GenTestPattern(void)
{
    int x, y;

    for(x = 0; x < 160; x++)
    {
        for(y = 0; y < 144; y++)
        {
            uint8_t c = x ^ y;
            t_Pixel pix;
            pix.r = c;
            pix.g = c;
            pix.b = c;
            PutPixel(x, y, pix);
        }
    }
}

void c_Canvas::PutPixel(uint8_t x, uint8_t y, t_Pixel data)
{
    pixels[160*y+x] = data;
}

void c_Canvas::InitGL(void)
{
    SetCurrent();
    //Set clear colour to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
}

