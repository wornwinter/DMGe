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
        void Exit(wxCommandEvent& event);

    private:
        c_GameBoy* gameboy;
        c_Canvas *glCanvas;
        wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
