#ifndef CANVAS_H
#define CANVAS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <wx/glcanvas.h>
#include <wx/dcclient.h>

class c_Canvas : public wxGLCanvas {
    private:
        bool init;
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

        void Render(void);
        void InitGL(void);

    DECLARE_EVENT_TABLE()
};

#endif // CANVAS_H
