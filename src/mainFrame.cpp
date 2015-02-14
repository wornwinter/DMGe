#include "mainFrame.h"

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    gameboy = new c_GameBoy("roms/tetris.gb");
    wxTimer* timer = new wxTimer(this, GB_TIMER);
    timer->Start(16); //Interval in milliseconds.
}

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
    EVT_TIMER(GB_TIMER, mainFrame::Tick)
wxEND_EVENT_TABLE()

void mainFrame::Tick(wxTimerEvent& event)
{
    gameboy->Run();
}
