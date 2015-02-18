#include "canvas.h"

c_Canvas::c_Canvas(wxWindow *parent, const wxWindowID id,
            const wxPoint &pos, const wxSize &sz,
            long style, const wxString &name)
            : wxGLCanvas(parent, (wxGLCanvas*)0, id, pos, sz, style, name),
            init(false)
{

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	SwapBuffers();
	event.Skip();
}

void c_Canvas::InitGL(void)
{
    SetCurrent();
    //Set clear colour to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

