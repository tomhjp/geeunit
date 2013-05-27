#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>

using namespace std;

// MyGLCanvas /////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_SCROLLWIN(MyGLCanvas::OnScroll)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()
  
int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod,network* network_mod,
		       const wxPoint& pos, const wxSize& size, long style, const wxString& name):
    wxGLCanvas(parent, id, pos, size, style, name, wxglcanvas_attrib_list)
  // Constructor - initialises private variables
{
    mmz = monitor_mod;
    nmz = names_mod;
    netz = network_mod;
    init = false;
    cyclesdisplayed = -1;
    SetScrollbar(wxVERTICAL,0,16,50);
    
    /* Find beginning of the list of devices */
    devlink dlink = netz->devicelist();
    while (dlink != NULL)
    {
        name_t did = dlink->id;
        namestring_t namestring = nmz->getName(did);
        cout << "Adding " << namestring << " to deviceNameVector" << endl;
        wxString devStr(namestring.c_str(), wxConvUTF8);  
        deviceNameVector.push_back(devStr);
        dlink = dlink->next;
    }
}

void MyGLCanvas::Render(wxString example_text, int cycles)
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.
{
    float y;
    unsigned int i,j;
    asignal s;
    int width, height,traceboxWidth,traceboxHeight;
    int numTraces = 10;
  
    int lenTrace = 20;
    int margin = 20;
    int unitWidth, unitHeight;
    int x;
    int labelWidth = 30;
    GetClientSize(&width,&height);
            
    traceboxWidth = width - 2*margin - labelWidth;
    traceboxHeight = height - 2*margin;            
    unitHeight = 20;
    int num = 1;

   
  

    
    if (cycles >= 0) cyclesdisplayed = cycles;

    SetCurrent();
  
    if (!init) 
    {
        InitGL();
        init = true;
    }
//    cout << "Cycles displayed: " << cyclesdisplayed << endl << \
//            "Monitor count: " << mmz->moncount() << endl;

    //getmonname(
    bool ok;
    for (int i=0; i<maxcycles; i++)
    {
        ok = mmz->getsignaltrace(0,i,s);
        
/*        if (ok)
            cout << s << endl;
        else
            cout << "Not ok" << endl;
*/
    }
    glClear(GL_COLOR_BUFFER_BIT);
    
    //If there are monitors then draw the first monitor signal, get trace from monitor class
    if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0))
    { 
        unitWidth = traceboxWidth / cyclesdisplayed;
        if (unitWidth > 30)
        {
            unitWidth = 30;
        }
        cout << "Matrix size = " << traceMatrix.size() << endl;
        for (int j=0;j<traceMatrix.size();j++)
        {   
            cout << "trace = " << j << endl;
            glColor3f(0.0, 0.0, 1.0);
            glBegin(GL_LINE_STRIP);
            for (i=0; i<cyclesdisplayed; i++) 
            {     
                s = traceMatrix[j][i];
                if (s==low)
                {
                    y = (traceboxHeight + margin - unitHeight - 2.5*unitHeight*j);
                    x = margin + labelWidth + unitWidth*i;
                    
                }
                 
                if (s==high)
                {
                    y = (traceboxHeight + margin - 2.5*unitHeight*j);
                    x = margin + labelWidth + unitWidth*i;
                }
                glVertex2f(x, y); 
                glVertex2f(x+unitWidth, y);
            }
            glEnd();
            
            y = (traceboxHeight-1 - 2.5*unitHeight*j);
            glColor3f(0.0, 0.0, 0.0);
            glRasterPos2f(margin/2,y);
            
            
            wxString traceText;
            traceText = monitorNameVector[j];
                      
            for (i = 0; i < traceText.Len() ; i++)
            {        
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, traceText[i]);
            }  
         }
       
    }
    
  // If there are no montiors then draw an artificial trace.
    else  
    {   
        for (int j=0;j<traceMatrix.size();j++)
        {
            
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_LINE_STRIP);
            for (i=0; i<cyclesdisplayed; i++) 
            {
                if (i%2)
                {
                    y = (traceboxHeight + margin - 2.5*unitHeight*j);
                    x = margin + labelWidth + unitWidth*i;
                }
                
                else
                {
                    y = (traceboxHeight + margin - unitHeight - 2.5*unitHeight*j);
                    x = (margin + labelWidth + unitWidth*i);
                }
                
                glVertex2f(x, y); 
                glVertex2f(x + unitWidth, y);                          
            }
            glEnd();
            
            y = (traceboxHeight-1 - 2.5*unitHeight*j);
            glColor3f(0.0, 0.0, 0.0);
            glRasterPos2f(margin/2,y);
            
            wxString traceText;
            traceText = monitorNameVector[j];
                      
            for (i = 0; i < traceText.Len() ; i++)
            {        
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, traceText[i]);
            }  
                
            
        }
    
    }
    


    // Example of how to use GLUT to draw text on the canvas
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(margin, 0.1*height);
    for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);
    
    // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
    glFlush();
    SwapBuffers();
}

void MyGLCanvas::InitGL()
  // Function to initialise the GL context
{
  int w, h;

  GetClientSize(&w, &h);
  SetCurrent();
  glDrawBuffer(GL_BACK);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1); 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void MyGLCanvas::OnPaint(wxPaintEvent& event)
  // Callback function for when the canvas is exposed
{
  int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  text.Printf(wxT("Canvas redrawn by OnPaint callback, canvas size is %d by %d"), w, h);
  Render(text,-1);
}

void MyGLCanvas::OnScroll(wxScrollWinEvent& event) 
{
  cout <<"position=" << event.GetPosition() << endl;

}


void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Callback function for when the canvas is resized
{
  wxGLCanvas::OnSize(event); // required on some platforms
  init = false;
  Refresh(); // required by some buggy nvidia graphics drivers,
  Update();  // harmless on other platforms!
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Callback function for mouse events inside the GL canvas
{
  wxString text;
  int w, h;;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) text.Printf(wxT("Mouse button %d pressed at %d %d"), event.GetButton(), event.m_x, h-event.m_y);
  if (event.ButtonUp()) text.Printf(wxT("Mouse button %d released at %d %d"), event.GetButton(), event.m_x, h-event.m_y);
  if (event.Dragging()) text.Printf(wxT("Mouse dragged to %d %d"), event.m_x, h-event.m_y);
  if (event.Leaving()) text.Printf(wxT("Mouse left window at %d %d"), event.m_x, h-event.m_y);

  if (event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text,-1);
}

void MyGLCanvas::populateMatrix()
{    
    
    for (int n=0; n < (mmz->moncount()); n++)
    {   
        
        name_t dev, outp;
        mmz->getmonname(n,dev,outp);
        namestring_t namestring = nmz->getName(dev);
        wxString devStr(namestring.c_str(), wxConvUTF8);   
        
        monitorNameVector.push_back(devStr);
        vector<asignal> emptyVector;
        traceMatrix.push_back(emptyVector);
        
        for (int i=0;i<cyclesdisplayed;i++)
        {
            asignal s;
            bool ok = mmz->getsignaltrace(n, i, s);
            if (ok)
            {
                traceMatrix[n].push_back(s);
            }
        }    
    }
}

void MyGLCanvas::setCyclesDisplayed(int c)
{
    cyclesdisplayed = c;
}
// MyFrame ///////////////////////////////////////////////////////////////////////////////////////



BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)

  EVT_BUTTON(RUN_BUTTON,  MyFrame::OnRunButton)
  EVT_BUTTON(CONT_BUTTON, MyFrame::OnContButton)

  EVT_BUTTON(ZAP_TRACE_BUTTON, MyFrame::OnButtonZap)
  EVT_BUTTON(ADD_TRACE_BUTTON, MyFrame::OnButtonAdd)

  EVT_BUTTON(SWITCH_BUTTON_1, MyFrame::OnButtonSwitch1)
  EVT_BUTTON(SWITCH_BUTTON_2, MyFrame::OnButtonSwitch2)

  EVT_COMBOBOX(ZAP_TRACE_COMBO_BOX, MyFrame::OnSelect)
  EVT_COMBOBOX(ADD_TRACE_COMBO_BOX, MyFrame::OnSelect)
  
  EVT_SPINCTRL(RUN_SPINCTRL,  MyFrame::OnSpin)
  EVT_SPINCTRL(CONT_SPINCTRL, MyFrame::OnSpin)

  EVT_TEXT_ENTER(COMMAND_LINE, MyFrame::OnText)
END_EVENT_TABLE()




MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, devices *devices_mod, monitor *monitor_mod, network *network_mod, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
    int numTraces = 10;
    int numSwitches = 3;
    SetIcon(wxIcon(wx_icon));
    
    nmz = names_mod;
    dmz = devices_mod;
    mmz = monitor_mod;
  
    if (nmz == NULL || dmz == NULL || mmz == NULL) 
    {
        cout << "Cannot operate GUI without names, devices and monitor classes" << endl;
        exit(1);
    }
    
    wxMenuBar *menuBar = new wxMenuBar;
    
    wxMenu *fileMenu = new wxMenu;
    wxMenu *editMenu = new wxMenu;
    wxMenu *viewMenu = new wxMenu;
    wxMenu *searchMenu = new wxMenu;
    wxMenu *toolsMenu = new wxMenu;
    
    fileMenu->Append(wxID_ABOUT, wxT("&About"));
    fileMenu->Append(wxID_EXIT, wxT("&Quit"));

    
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(viewMenu, wxT("&View"));
    menuBar->Append(searchMenu, wxT("&Search"));
    menuBar->Append(toolsMenu, wxT("&Tools"));

    
    SetMenuBar(menuBar);

  // Define the Trace Canvas
    canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, network_mod);
  
  // Define the sizers required  
    wxBoxSizer *frame_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sidebar_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *combo_sizer = new wxBoxSizer(wxHORIZONTAL);


    wxString traceList[mmz->moncount()];
    
  // Make the zap/add combobox strings
    for (int j=0;j<mmz->moncount();j++)
    {
        name_t dev;
        name_t outp;
        mmz->getmonname(j,dev,outp);
        namestring_t namestring = nmz->getName(dev);
        wxString devStr(namestring.c_str(), wxConvUTF8);   
        traceList[j] = devStr;
    }

  // Make the switch combo-box strings
    wxString switchList[numSwitches];
    for (int i=0;i<numSwitches;i++)
    {
        wxString text;
        text.Printf(wxT("Switch %d"),i+1);
        switchList[i] = text;
    }  




 
  
  // DEFINITIONS OF BUTTONS, CONTROLS AND SIZERS
  // *********************************************************************************************************************************
  
  //Define Zap Controls
    zapTraceComboBox = new wxComboBox(this,ZAP_TRACE_COMBO_BOX, wxT("Choose trace to ZAP!"),wxDefaultPosition, wxDefaultSize,mmz->moncount(),traceList);
    zapTraceButton = new wxButton(this, ZAP_TRACE_BUTTON, wxT("ZAP"));
    
  // Define Add Controls

    addTraceComboBox = new wxComboBox(this,ADD_TRACE_COMBO_BOX, wxT("Choose trace to Add"),wxDefaultPosition, wxDefaultSize,mmz->moncount(),traceList);
    addTraceButton = new wxButton(this, ADD_TRACE_BUTTON, wxT("ADD"));
    
  // Define Switch Controls
    switchComboBox = new wxComboBox(this,SWITCH_COMBO_BOX, wxT("Choose Switch "), wxDefaultPosition, wxDefaultSize, numSwitches, switchList); 
    switchButton1 = new wxButton(this, SWITCH_BUTTON_1, wxT("SWITCH TO 1"));
    switchButton2 = new wxButton(this, SWITCH_BUTTON_2, wxT("SWITCH TO 0"));
    
  // Define Other buttons  
    runButton = new wxButton(this, RUN_BUTTON, wxT("Run"));
    contButton = new wxButton(this, RUN_BUTTON, wxT("Continue"));
    runSpin = new wxSpinCtrl(this, RUN_SPINCTRL, wxString(wxT("10")));
    contSpin = new wxSpinCtrl(this, CONT_SPINCTRL, wxString(wxT("10")));
    commandLine = new wxTextCtrl(this, COMMAND_LINE, wxT(""), wxDefaultPosition, wxSize(150,30), wxTE_PROCESS_ENTER|wxTE_MULTILINE);
    commandLine->WriteText(wxT("#"));
    runStaticText = new wxStaticText(this, wxID_ANY, wxT("Cycles"));
    contStaticText = new wxStaticText(this, wxID_ANY, wxT("Cycles"));
    
  // Place Zap Controls
    wxBoxSizer *zap_sizer = new wxBoxSizer(wxVERTICAL);
    zap_sizer->Add(zapTraceComboBox,0,0,0);
    zap_sizer->Add(zapTraceButton,  0,wxALIGN_CENTRE,0);
    
  // Place Add controls
    wxBoxSizer *add_sizer = new wxBoxSizer(wxVERTICAL);
    add_sizer->Add(addTraceComboBox,0,0,0);
    add_sizer->Add(addTraceButton,  0,wxALIGN_CENTRE,0);
  
  // Place switch combo
    wxBoxSizer *switch_sizer = new wxBoxSizer(wxVERTICAL);
    switch_sizer->Add(switchComboBox,0,wxALIGN_CENTRE | wxEXPAND | wxTOP,30);
    
  // Place switch buttons  
    wxBoxSizer *switchButton_sizer = new wxBoxSizer(wxHORIZONTAL);
    switchButton_sizer->Add(switchButton1,0,0,0);
    switchButton_sizer->Add(switchButton2,0,0,0);
    
  // Place Buttons  
    wxBoxSizer *run_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    run_button_sizer->Add(runButton, 0, wxTOP, 30);
    run_button_sizer->Add(runSpin, 0, wxEXPAND | wxTOP, 30);
    run_button_sizer->Add(runStaticText,0,wxALIGN_CENTRE|wxTOP,30);
    
  // Place Buttons  
    wxBoxSizer *cont_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    cont_button_sizer->Add(contButton, 0, wxTOP, 30);
    cont_button_sizer->Add(contSpin, 0, wxEXPAND | wxTOP, 30);
    cont_button_sizer->Add(contStaticText,0,wxALIGN_CENTRE|wxTOP,30);

  // *********************************************************************************************************************************

  
  
  
  // NESTED SIZERS FOR PLACEMENT OF CONTROL LOOPS
  //**********************************************************************************************************************************
  frame_sizer->Add(topsizer,5, wxEXPAND | wxTOP|wxLEFT|wxBOTTOM,30);                             
  frame_sizer->Add(commandLine,1,wxEXPAND | wxALL,20);                                             
  
  topsizer->Add(canvas,3,wxEXPAND,0);
  topsizer->Add(sidebar_sizer,2,wxALIGN_CENTRE,0);
  
  sidebar_sizer->Add(combo_sizer,0,wxALIGN_CENTRE);
  sidebar_sizer->Add(switch_sizer, 0, wxALIGN_CENTRE);
  sidebar_sizer->Add(run_button_sizer,0,wxALIGN_CENTRE);
  sidebar_sizer->Add(cont_button_sizer,0,wxALIGN_CENTRE);
  
  combo_sizer->Add(zap_sizer, 0, wxALIGN_CENTER);
  combo_sizer->Add(add_sizer, 0, wxALIGN_CENTRE);
  
  switch_sizer->Add(switchButton_sizer,0,wxALIGN_CENTRE);
//************************************************************************************************************************************
  

  SetSizeHints(1000, 400);
  SetSizer(frame_sizer);
  
  runnetwork(10);
  mmz->displaysignals();
}

void MyFrame::OnExit(wxCommandEvent &event)
  // Callback for the exit menu item
{
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
  // Callback for the about menu item
{
    wxString text = wxT("Example");
}


// Callback for the run button
void MyFrame::OnRunButton(wxCommandEvent &event) 
{
  int n, ncycles;

  cyclescompleted = 0;
  mmz->resetmonitor ();
  runnetwork(runSpin->GetValue());
  canvas->setCyclesDisplayed(runSpin->GetValue());
  mmz->displaysignals();
  
  canvas->populateMatrix();
  cout << canvas->traceMatrix.size() << endl;
  canvas->Render(wxT("Run button pressed"), cyclescompleted);
}

// Callback for the continue button
void MyFrame::OnContButton(wxCommandEvent &event)
{
    //cyclesdisplayed += contSpin->GetValue();
}

void MyFrame::OnButtonZap(wxCommandEvent &event)
  // Callback for Zap PushButton
{

    wxString selectionStr = zapTraceComboBox->GetStringSelection();
    wxString text;
    text.Printf(wxT("%s removed"),selectionStr.c_str());
    
    int selection = zapTraceComboBox->GetSelection();

    canvas->traceMatrix.erase(canvas->traceMatrix.begin() + selection);
    canvas->monitorNameVector.erase(canvas->monitorNameVector.begin() + selection);

    canvas->Render(text,-1);
    
    zapTraceComboBox->Delete(selection);
        
}

void MyFrame::OnButtonAdd(wxCommandEvent &event)
  // Callback for second pushbutton
{

    wxString selectionStr = zapTraceComboBox->GetStringSelection();
    wxString text;
    text.Printf(wxT("%s removed"),selectionStr.c_str());
    
    int selection = zapTraceComboBox->GetSelection();
    canvas->traceMatrix.erase(canvas->traceMatrix.begin() + selection);
    canvas->populateMatrix();
    
    canvas->Render(text,-1);
    zapTraceComboBox->Delete(selection);
    
}

void MyFrame::OnButtonSwitch1(wxCommandEvent &event)
  // Callback for second pushbutton
{
}

void MyFrame::OnButtonSwitch2(wxCommandEvent &event)
  // Callback for second pushbutton
{
}

void MyFrame::OnSpin(wxSpinEvent &event)
  // Callback for the spin control
{
  wxString text;
  text.Printf(wxT("New spinctrl value %d"), event.GetPosition());
  canvas->Render(text,-1);
}

void MyFrame::OnText(wxCommandEvent &event)
  // Callback for the text entry field
{
    int numberOfLines = commandLine->GetNumberOfLines();
    wxString text;
    text.Printf(wxT("cmd: %s"), commandLine->GetLineText(numberOfLines).c_str());
    commandLine->WriteText(wxT("\n# "));
    canvas->Render(text,-1);
}

void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
    bool ok = true;
    int n = ncycles;

    while ((n > 0) && ok) 
    {
        dmz->executedevices (ok);
        
        if (ok) 
            {
               n--;
               mmz->recordsignals ();
            }
        else
        cout << "Error: network is oscillating" << endl;
    }
    
  if (ok) 
    cyclescompleted = cyclescompleted + ncycles;
  else 
    cyclescompleted = 0;
}

void MyFrame::aboutfunction(wxString traceStr, wxString switchStr)
{
  wxString message;
  message.Printf(wxT("Trace Selected: %s \nSwitch Selected: %s"),traceStr.c_str(),switchStr.c_str());
  wxMessageDialog about(this,message,wxT("About"), wxICON_INFORMATION | wxOK);
  about.ShowModal();
  return;
}

void MyFrame::OnSelect(wxCommandEvent &event)
{
    wxString trace = event.GetString();
}


