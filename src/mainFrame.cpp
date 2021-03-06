#include "mainFrame.h"
#include "canvas.h"
#include "logo.xpm"
#include "GameBoy.h"

mainFrame::mainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size, frameStyle)
{
    //File Menu
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(MENU_OPEN, "&Open");
    menuFile->Append(MENU_EXIT, "&Exit");

    menuEmu = new wxMenu;
    menuEmu->Append(MENU_START, "&Start");
    menuEmu->Append(MENU_STOP, "&Stop");
    menuEmu->AppendCheckItem(MENU_SKIP, "&Skip BIOS");

    wxMenu* menuVid = new wxMenu;
    wxMenu* menuScale = new wxMenu;

    menuVid->AppendSubMenu(menuScale, "&Scale");
    menuScale->Append(MENU_SCALE1, "&1x");
    menuScale->Append(MENU_SCALE2, "&2x");
    menuScale->Append(MENU_SCALE3, "&3x");
    menuScale->Append(MENU_SCALE4, "&4x");

    #ifdef _DEBUG
    menuEmu->Append(MENU_SUPPRESS, "Suppress debug output. (Not implemented)");
    #endif // _DEBUG

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEmu, "&Emulation");
    menuBar->Append(menuVid, "&Video");

    SetMenuBar(menuBar);

    wxIcon mainIcon(logo_xpm);
    SetIcon(mainIcon);

    //Use a panel for absolute positioning. Temporary.
    wxPanel* panel = new wxPanel(this, -1);

    glCanvas = new c_Canvas(panel, -1, wxDefaultPosition, wxSize(160, 144));


    //gameboy = new c_GameBoy("roms/opus5.gb", glCanvas);
    //gameboy = new c_GameBoy("roms/TEST.GB", glCanvas);
    //gameboy = new c_GameBoy("roms/pred.gb", glCanvas);
    //gameboy = new c_GameBoy("roms/instr_timing.gb", glCanvas);
    gameboy = new c_GameBoy("roms/tetris.gb", glCanvas);

    gameboy->pause = true;

    emuthread = new boost::thread(boost::bind(&c_GameBoy::Run, gameboy));

    //wxTimer* timer = new wxTimer(this, GB_TIMER);
    //timer->Start(1); //Interval in milliseconds.
}

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)
    //EVT_TIMER(GB_TIMER, mainFrame::Tick)
    //EVT_IDLE(mainFrame::Tick)
    EVT_MENU(MENU_START, mainFrame::StartEmulation)
    EVT_MENU(MENU_STOP, mainFrame::StopEmulation)
    EVT_MENU(MENU_OPEN, mainFrame::OpenROM)
    EVT_MENU(MENU_SCALE1, mainFrame::Scale1)
    EVT_MENU(MENU_SCALE2, mainFrame::Scale2)
    EVT_MENU(MENU_SCALE3, mainFrame::Scale3)
    EVT_MENU(MENU_SCALE4, mainFrame::Scale4)
    EVT_MENU(MENU_EXIT, mainFrame::Exit)
    EVT_MENU(MENU_SKIP, mainFrame::Skip)
wxEND_EVENT_TABLE()

void mainFrame::Tick(wxIdleEvent& event)
{
    gameboy->Run();
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

void mainFrame::Skip(wxCommandEvent& event)
{
    wxMenuItem *skipitem = menuEmu->FindItem(MENU_SKIP);
    gameboy->SkipBios(skipitem->IsChecked());
}

void mainFrame::OpenROM(wxCommandEvent& event)
{
    wxFileDialog* open = new wxFileDialog(this, _("Open ROM"), "", "", _("GameBoy files (.gb) | *.gb|ROM Files (.rom) | *.rom|ZIP Files (.zip)|*.zip"), wxFD_OPEN, wxDefaultPosition);

    if(open->ShowModal() == wxID_CANCEL)
        return;

    wxFileName fpath = open->GetPath();
    std::string rname = std::string(fpath.GetFullName().mb_str());
    std::string fname = "roms\\" + rname;
    gameboy->LoadROM(fname.c_str());
}

void mainFrame::Scale1(wxCommandEvent& event)
{
    //Approximate frame values
    this->SetSize(160 + 10, 144 + 50);
    glCanvas->Scale(SCALE1x);
    glCanvas->Refresh();
}

void mainFrame::Scale2(wxCommandEvent& event)
{
    //Approximate frame values
    this->SetSize((160 * SCALE2x) + 10, (144 * SCALE2x) + 50);
    glCanvas->Scale(SCALE2x);
    glCanvas->Refresh();
}

void mainFrame::Scale3(wxCommandEvent& event)
{
    //Approximate frame values
    this->SetSize((160 * SCALE3x) + 10, (144 * SCALE3x) + 50);
    glCanvas->Scale(SCALE3x);
    glCanvas->Refresh();
}

void mainFrame::Scale4(wxCommandEvent& event)
{
    //Approximate frame values
    this->SetSize((160 * SCALE4x) + 10, (144 * SCALE4x) + 50);
    glCanvas->Scale(SCALE4x);
    glCanvas->Refresh();
}

void mainFrame::Exit(wxCommandEvent& event)
{
    exit(EXIT_SUCCESS);
}
