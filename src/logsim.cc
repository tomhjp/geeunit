#include "logsim.h"
#include "userint.h"
#include "gui.h"
#include <GL/glut.h>
#include <string>

#define USE_GUI

IMPLEMENT_APP(MyApp)
  
bool MyApp::OnInit()
  // This function is automatically called when the application starts
{
  if (argc != 2) { // check we have one command line argument
    wcout << "Usage:      " << argv[0] << " [filename]" << endl;
    exit(1);
  }

  // Construct the six classes required by the innards of the logic simulator
    nmz = new names();
    netz = new network(nmz);
    dmz = new devices(nmz, netz);
    mmz = new monitor(nmz, netz);
    smz = new scanner_t(string(wxString(argv[1]).mb_str()));
    pmz = new parser(netz, dmz, mmz, smz, nmz);

    symbol_t symbol;
    symbol.symboltype = startfsym;      // arbitrary symboltype that is not eofsym
    
    /* Read through whole file outputting one symbol and its type at a time */
    while (symbol.symboltype != eofsym)
    {
        smz->nextSymbol(symbol);
        pmz->readin(symbol);
    }

    if (1) { // check the logic file parsed correctly, will be check that errorvector is empty
#ifdef USE_GUI
    // glutInit cannot cope with Unicode command line arguments, so we pass
    // it some fake ASCII ones instead
    char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
    // Construct the GUI
    MyFrame *frame = new MyFrame(NULL, wxT("Logic simulator"), wxDefaultPosition,  wxSize(1500, 800), nmz, dmz, mmz, netz);
    frame->Show(true);
    return(true); // enter the GUI event loop
#else
    // Construct the text-based interface
    userint umz(nmz, dmz, mmz);
    umz.userinterface();
#endif /* USE_GUI */
  }
  return(false); // exit the application
}
