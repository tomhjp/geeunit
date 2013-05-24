#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>

using namespace std;

// MyGLCanvas /////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()
  
int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod,
		       const wxPoint& pos, const wxSize& size, long style, const wxString& name):
    wxGLCanvas(parent, id, pos, size, style, name, wxglcanvas_attrib_list)
  // Constructor - initialises private variables
{
    mmz = monitor_mod;
    nmz = names_mod;
    init = false;
    cyclesdisplayed = -1;
    
    for (int i=0;i<10;i++)
    {
        traceVector.push_back(i);
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
    unitWidth = traceboxWidth / lenTrace;
    unitHeight = 20;
    int num = 1;

   
  

    
    if (cycles >= 0) cyclesdisplayed = cycles;

    SetCurrent();
  
    if (!init) 
    {
        InitGL();
        init = true;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0))
    { // draw the first monitor signal, get trace from monitor class
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINE_STRIP);
        for (i=0; i<cyclesdisplayed; i++) 
        {
          if (mmz->getsignaltrace(0, i, s)) 
          {
              if (s==low) y = 100.0;
              if (s==high) y = 130.0;
              
              glVertex2f(20*i+10.0, y); 
              glVertex2f(20*i+30.0, y);
          }
        }
        glEnd();
    }
    else // draw an artificial trace 
    {   
        for (int j=0;j<traceVector.size();j++)
        {
            
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_LINE_STRIP);
            for (i=0; i<lenTrace; i++) 
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
            traceText.Printf(wxT("CLK%d"),traceVector[j]);
                      
            for (i = 0; i < traceText.Len() ; i++)
            {        
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, traceText[i]);
            }
                
            
        }
    
    }
    


    // Example of how to use GLUT to draw text on the canvas
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(margin, 0.15*height);
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

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////

wxString str = wxT("hello");

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_BUTTON(MY_BUTTON_ID, MyFrame::OnButton)
  EVT_BUTTON(ZAP_BUTTON, MyFrame::OnButtonZap)
  EVT_COMBOBOX(ZAP_TRACE_COMBO_BOX, MyFrame::OnSelect)
  EVT_COMBOBOX(ADD_TRACE_COMBO_BOX, MyFrame::OnSelect)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
  EVT_TEXT_ENTER(MY_TEXTCTRL_ID, MyFrame::OnText)
END_EVENT_TABLE()




MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, devices *devices_mod, monitor *monitor_mod, long style):
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

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_ABOUT, wxT("&About"));
    fileMenu->Append(wxID_EXIT, wxT("&Quit"));
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, wxT("&File"));
    SetMenuBar(menuBar);

    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
    canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod);
    topsizer->Add(canvas, 1, wxEXPAND | wxTOP|wxLEFT|wxBOTTOM, 300);

    wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, MY_BUTTON_ID, wxT("Run")), 0, wxALL, 10);
  
    wxString traceList[canvas->traceVector.size()];
    for (int i=0;i<canvas->traceVector.size();i++)
    {
        wxString text;
        text.Printf(wxT("Trace %d"),i+1);
        traceList[i] = text;
    }
  
    wxString switchList[numSwitches];
    for (int i=0;i<numSwitches;i++)
    {
        wxString text;
        text.Printf(wxT("Switch %d"),i+1);
        switchList[i] = text;
    }  


 
  
  
  // *********************************************************************************************
  zapTraceComboBox = new wxComboBox(this,ZAP_TRACE_COMBO_BOX, wxT("Choose trace to ZAP!"),wxDefaultPosition, wxDefaultSize,canvas->traceVector.size(),traceList);
  switchComboBox = new wxComboBox(this,SWITCH_COMBO_BOX, wxT("Choose Switch to change"), wxDefaultPosition, wxDefaultSize, numSwitches, switchList); 
  addTraceComboBox = new wxComboBox(this,ADD_TRACE_COMBO_BOX, wxT("Choose trace to Add"),wxDefaultPosition, wxDefaultSize,canvas->traceVector.size(),traceList);
  
  wxBoxSizer *combo_sizer = new wxBoxSizer(wxHORIZONTAL);
  combo_sizer->Add(zapTraceComboBox,0,wxTOP|wxBOTTOM,0);
  combo_sizer->Add(addTraceComboBox,0,wxTOP|wxBOTTOM,0);
  SetSizer(combo_sizer);
  //combo_sizer->Add(new wxButton(this, ZAP_BUTTON, wxT("ZAP")), 0, wxLEFT|wxRIGHT ,10);
  //combo_sizer->Add(switchComboBox,0,wxLEFT|wxRIGHT,10);
  //combo_sizer->Add(new wxButton(this, SWITCH_BUTTON, wxT("Switch")), 0, wxLEFT|wxRIGHT ,10);
  //SetSizer(combo_sizer);
  // *********************************************************************************************

  
  button_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Cycles")), 0, wxALL, 10);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString(wxT("10")));
  button_sizer->Add(spin, 0 , wxALL, 10);

  button_sizer->Add(new wxTextCtrl(this, MY_TEXTCTRL_ID, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER), 0 , wxALL, 10);
  topsizer->Add(button_sizer, 0, wxALIGN_CENTER);

  SetSizeHints(400, 400);
  SetSizer(topsizer);
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

void MyFrame::OnButton(wxCommandEvent &event)
  // Callback for the push button
{
  int n, ncycles;

  cyclescompleted = 0;
  mmz->resetmonitor ();
  runnetwork(spin->GetValue());
  canvas->Render(wxT("Run button pressed"), cyclescompleted);
}

void MyFrame::OnButtonZap(wxCommandEvent &event)
  // Callback for second pushbutton
{
    
    wxString selectionStr = zapTraceComboBox->GetStringSelection();
    wxString text;
    text.Printf(wxT("%s removed"),selectionStr.c_str());
    
    int selection = zapTraceComboBox->GetSelection();
    canvas->traceVector.erase(canvas->traceVector.begin() + selection);

    canvas->Render(text,-1);
    zapTraceComboBox->Delete(selection);
        
}

void MyFrame::OnButtonSwitch(wxCommandEvent &event)
  // Callback for second pushbutton
{
    /*
    wxString selectionStr = zapTraceComboBox->GetStringSelection();
    wxString text;
    text.Printf(wxT("%s removed"),selectionStr.c_str());
    
    int selection = zapTraceComboBox->GetSelection();
    canvas->traceVector.erase(canvas->traceVector.begin() + selection);

    canvas->Render(text,-1);
    zapTraceComboBox->Delete(selection);
    */    
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
  wxString text;

  text.Printf(wxT("New text entered %s"), event.GetString().c_str());
  canvas->Render(text,-1);
}

void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
  bool ok = true;
  int n = ncycles;

  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      cout << "Error: network is oscillating" << endl;
  }
  if (ok) cyclescompleted = cyclescompleted + ncycles;
  else cyclescompleted = 0;
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

