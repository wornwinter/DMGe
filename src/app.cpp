#include "app.h"
#include "mainFrame.h"




bool DMGeApp::OnInit()
{
    mainFrame* frame = new mainFrame("DMGe :: GameBoy Emulator", wxPoint(50,50), wxSize(175, 194));
    frame->Show(true);
    return true;
}

