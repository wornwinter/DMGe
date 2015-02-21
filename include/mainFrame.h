#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <stdlib.h> //Exit signal stuff



#define GB_TIMER 1

#define MENU_OPEN 2
#define MENU_EXIT 3

#define MENU_START 4
#define MENU_STOP 5
#define MENU_SUPPRESS 6

#define MENU_SCALE1 7
#define MENU_SCALE2 8
#define MENU_SCALE3 9
#define MENU_SCALE4 10

#define frameStyle (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)

class c_GameBoy;
class c_Canvas;


//Main window class definition.
class mainFrame : public wxFrame
{
    public:
        mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        void Tick(wxIdleEvent& event);
        void StartEmulation(wxCommandEvent& event);
        void StopEmulation(wxCommandEvent& event);
        void Scale1(wxCommandEvent& event);
        void Scale2(wxCommandEvent& event);
        void Scale3(wxCommandEvent& event);
        void Scale4(wxCommandEvent& event);
        void Exit(wxCommandEvent& event);

    private:
        c_GameBoy* gameboy;
        c_Canvas *glCanvas;
        wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
