#include "guitest.h"
#include "gui.h"
#include <GL/glut.h>

IMPLEMENT_APP(MyApp)
  
bool MyApp::OnInit()
  // This function is automatically called when the application starts
{
  // Construct the four classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);

  bool ok = false;
  devicekind dkind = aclock;
  int variant = 2;
  name_t did;
  name_t oid = blankname;

  did = nmz->lookup("myclk");
  //cout << did << endl;
  dmz->makedevice(dkind, did, variant, ok);
  /*if (ok)
    cout << "success: Made device" << endl;
  else
    cout << "fail: Did not make device" << endl;*/
  netz->checknetwork(ok);
  /*if (ok)
    cout << "Network ok" << endl;
  else
    cout << "Network NOT ok" << endl;*/
  //mmz->makemonitor(did, oid, ok);
  /*if (ok)
    cout << "SUCCESS: Made monitor ok" << endl;
  else
    cout << "Failed to make monitor" << endl;*/

  // glutInit cannot cope with Unicode command line arguments, so we pass
  // it some fake ASCII ones instead
  char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
  // Construct the GUI
  MyFrame *frame = new MyFrame(NULL, wxT("Logic simulator"), wxDefaultPosition,  wxSize(1500, 800), nmz, dmz, mmz);
  frame->Show(true);
  return(true); // enter the GUI event loop
}
