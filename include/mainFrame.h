#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include "GameBoy.h"

#define GB_TIMER 1

//Main window class definition.
class mainFrame : public wxFrame
{
    public:
        mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        void Tick(wxTimerEvent& event);

    private:
        c_GameBoy* gameboy;
        wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
