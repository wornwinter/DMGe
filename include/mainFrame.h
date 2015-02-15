#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include "GameBoy.h"

#define GB_TIMER 1

#define MENU_OPEN 2
#define MENU_EXIT 3

#define MENU_START 4
#define MENU_STOP 5

//Main window class definition.
class mainFrame : public wxFrame
{
    public:
        mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        void Tick(wxTimerEvent& event);
        void StartEmulation(wxCommandEvent& event);
        void StopEmulation(wxCommandEvent& event);

    private:
        c_GameBoy* gameboy;
        wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
