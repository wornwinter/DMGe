#include "canvas.h"
#include "debug.h"

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
    EVT_SIZE(c_Canvas::OnSize)
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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
    glTranslatef(0.0f, 0.0f, -6.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, &pixels);

	glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
        glTexCoord2f(1.0, 0.0);
        glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
        glTexCoord2f(1.0, 1.0);
        glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
    glEnd();

	SwapBuffers();
}

void c_Canvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event);

    Refresh(true);
}


void c_Canvas::PutPixel(uint8_t x, uint8_t y, t_Pixel pix)
{
    pixels[160*y+x] = pix;
}

void c_Canvas::InitGL(void)
{
    SetCurrent();
    glViewport(0, 0, 160, 144);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);

	glGenTextures(GL_TEXTURE_2D, &tex);
}

//Scale the graphics up so we can play at a larger resolution
void c_Canvas::Scale(uint8_t factor)
{
    this->SetSize(160 * factor, 144 * factor); //Reset canvas size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); //Reset the projection matrix
    glViewport(0, 0, 160* factor, 144 * factor);
}

