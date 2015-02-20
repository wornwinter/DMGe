#include "mainFrame.h"
#include "canvas.h"
#include "logo.xpm"
#include "GameBoy.h"

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    //File Menu
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(MENU_OPEN, "&Open");
    menuFile->Append(MENU_EXIT, "&Exit");

    wxMenu* menuEmu = new wxMenu;
    menuEmu->Append(MENU_START, "&Start");
    menuEmu->Append(MENU_STOP, "&Stop");

    #ifdef _DEBUG
    menuEmu->Append(MENU_SUPPRESS, "Suppress debug output. (Not implemented)");
    #endif // _DEBUG

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEmu, "&Emulation");

    SetMenuBar(menuBar);

    wxIcon mainIcon(logo_xpm);
    SetIcon(mainIcon);

    //Use a panel for absolute positioning. Temporary.
    wxPanel* panel = new wxPanel(this, -1);

    glCanvas = new c_Canvas(panel, -1, wxDefaultPosition, wxSize(160, 144));


    gameboy = new c_GameBoy("roms/tetris.gb", glCanvas);
    gameboy->pause = true;

    //wxTimer* timer = new wxTimer(this, GB_TIMER);
    //timer->Start(1); //Interval in milliseconds.
}

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
    //EVT_TIMER(GB_TIMER, mainFrame::Tick)
    EVT_IDLE(mainFrame::Tick)
    EVT_MENU(MENU_START, mainFrame::StartEmulation)
    EVT_MENU(MENU_STOP, mainFrame::StopEmulation)
    EVT_MENU(MENU_EXIT, mainFrame::Exit)
wxEND_EVENT_TABLE()

void mainFrame::Tick(wxIdleEvent& event)
{
    gameboy->Run();
    //Redraw canvas.
    glCanvas->Refresh();
    if(!gameboy->pause)
        event.RequestMore(true);
}

void mainFrame::StartEmulation(wxCommandEvent& event)
{
    gameboy->pause = false;
}

void mainFrame::StopEmulation(wxCommandEvent& event)
{
    gameboy->pause = true;
}

void mainFrame::Exit(wxCommandEvent& event)
{
    exit(EXIT_SUCCESS);
}
