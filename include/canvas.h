#ifndef CANVAS_H
#define CANVAS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <wx/glcanvas.h>
#include <wx/dcclient.h>

class c_Canvas : public wxGLCanvas {

    typedef struct {
        uint8_t r, g, b;
    } t_Pixel;

    private:
        bool init;
        t_Pixel pixels[160*144];
    public:
        c_Canvas(wxWindow *parent,
                    const wxWindowID id = -1,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &sz = wxDefaultSize,
                    long style = 0,
                    const wxString &name = "Canvas");
        virtual ~c_Canvas(void);

        void OnPaint(wxPaintEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnEraseBackGround(wxEraseEvent& event);
        void OnEnterWindow(wxMouseEvent& event);

        void InitGL(void);

        void GenTestPattern(void);
        void PutPixel(uint8_t x, uint8_t y, t_Pixel data);

    DECLARE_EVENT_TABLE()
};

#endif // CANVAS_H
