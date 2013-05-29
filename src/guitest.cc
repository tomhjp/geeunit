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

  did = nmz->lookup("clk_1");
  dmz->makedevice(dkind, did, 1, ok);
  mmz->makemonitor(did, oid, ok);
  
  did = nmz->lookup("clk_2");
  dmz->makedevice(dkind, did, variant, ok);
  mmz->makemonitor(did, oid, ok);
  
  did = nmz->lookup("clk_3");
  dmz->makedevice(dkind, did, 3, ok);
  
  dkind = aswitch;
  did = nmz->lookup("switch1");
  dmz->makedevice(dkind, did, 0, ok);
  mmz->makemonitor(did, oid, ok);
  
  did = nmz->lookup("switch2");
  dmz->makedevice(dkind, did, 1, ok);
  mmz->makemonitor(did, oid, ok);
  
  did = nmz->lookup("switch3");
  dmz->makedevice(dkind, did, 0, ok);
  
  dkind = aclock;
  did = nmz->lookup("clk_4");
  dmz->makedevice(dkind, did, 3, ok);
  mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_5");
  dmz->makedevice(dkind, did, 4, ok);
  mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_6");
  dmz->makedevice(dkind, did, 5, ok);
  mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_7");
  dmz->makedevice(dkind, did, 6, ok);
  //mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_8");
  dmz->makedevice(dkind, did, 7, ok);
  //mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_9");
  dmz->makedevice(dkind, did, 3, ok);
  //mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_10");
  dmz->makedevice(dkind, did, 2, ok);
  //mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_11");
  dmz->makedevice(dkind, did, 1, ok);
  //mmz->makemonitor(did, oid, ok);
  did = nmz->lookup("clk_12");
  dmz->makedevice(dkind, did, 4, ok);
  //mmz->makemonitor(did, oid, ok);
  
  /*if (ok)
    cout << "SUCCESS: Made monitor ok" << endl;
  else
    cout << "Failed to make monitor" << endl;*/

  // glutInit cannot cope with Unicode command line arguments, so we pass
  // it some fake ASCII ones instead
  char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
  // Construct the GUI
  MyFrame *frame = new MyFrame(NULL, wxT("Logic simulator"), wxDefaultPosition,  wxSize(1500, 800), nmz, dmz, mmz, netz);
  frame->Show(true);
  return(true); // enter the GUI event loop
}
