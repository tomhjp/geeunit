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
    CANVAStotalCycles = 0;
    CANVAScyclesCompleted = 0;
    vertCanvasPosition = 0;
    horizCanvasPosition = 0;
    unitHeight = 20;
    numDtypes = 0;
    clearToDraw = true;

    /* Populate deviceNameVector with the wxString names of all devices in the network */
    devlink dlink = netz->devicelist();     // Find beginning of the list of devices
    while (dlink != NULL)
    {
        name_t did = dlink->id;
        namestring_t namestring = nmz->getName(did);
        devicekind dkind = netz->netzdevkind(did);
        if (dkind == dtype)
            numDtypes++;
        wxString devStr(namestring.c_str(), wxConvUTF8);
        deviceNameVector.push_back(devStr);
        dlink = dlink->next;
    }
}

void MyGLCanvas::Render(int totalCycles)
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.
{

  // Initialise Variables that Render needs
    float y;
    unsigned int i,j,c,t;
    asignal s;
    float unitWidth;
    int x,width, height,margin,labelWidth,traceWidth,traceMargin,traceboxWidth,traceboxHeight;

    GetClientSize(&width,&height);
    margin = 20;
    labelWidth = 50;
    traceWidth = width - 2*margin - labelWidth - 2*traceMargin;
    traceMargin = 3;
    unitHeight = 20;
    traceboxWidth = width - 2*margin - labelWidth - 2*traceMargin;
    traceboxHeight = height - 2*margin - 2*traceMargin;

//    if (unitWidth < 2)
        unitWidth = 5;

    SetCurrent();

    if (!init)
    {
        InitGL();
        init = true;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
    if (clearToDraw)
    {
        //If there are monitors then draw the first monitor signal, get trace from monitor class
        if ((totalCycles > 0) && (mmz->moncount() > 0))
        {
            int offset = 0;
            
            if (unitWidth > 30)
            {
                unitWidth = 30;
            }

            for (t=0; t<traceMatrix.size(); t++)
            {
                glColor3f(0.9, 0.9, 0.9);

                glBegin(GL_QUADS);
                x = (margin + labelWidth);
                y = (traceboxHeight + margin - 2.5*unitHeight*t + vertCanvasPosition*5 + 3);

                glVertex2f(x,y);
                glVertex2f(x,y-unitHeight-6);
                glVertex2f(width,y - unitHeight-2*traceMargin);
                glVertex2f(width,y);
                glEnd();

                glColor3f(0.0, 0.0, 1.0);

                glBegin(GL_LINE_STRIP);
                    bool skipLow = false, skipHigh = false;
                    for (c=0; c<traceMatrix[0].size(); c++)
                    {
                        s = traceMatrix[t][c];
                        if (s==low)
                        {
                            y = (traceboxHeight + margin - unitHeight - 2.5*unitHeight*t + vertCanvasPosition*5);
                            x = margin + labelWidth + traceMargin + unitWidth*c - horizCanvasPosition*unitWidth;
                        }

                        if (s==high)
                        {
                            y = (traceboxHeight + margin - 2.5*unitHeight*t + vertCanvasPosition*5);
                            x = margin + labelWidth + traceMargin + unitWidth*c - horizCanvasPosition*unitWidth;
                        }
                        
                        if (x > margin + labelWidth + traceMargin)
                            glVertex2f(x,y);
                        if (x + unitWidth > margin + labelWidth + traceMargin)
                            glVertex2f(x+unitWidth, y);
                        
                    }
                glEnd();
            }
        

            int axisSpacing = 5;
            if (CANVAStotalCycles > 1000)
                axisSpacing = 10;

            for (t=0; t<traceMatrix.size(); t++)
            {
                for (c=0; c<traceMatrix[0].size(); c++)
                {
                    if (c%axisSpacing == 0)
                    {
                        wxString axisLabel;
                        axisLabel.Printf(wxT("%d"),c);
                        x = margin + labelWidth + traceMargin + unitWidth*c - 6 - horizCanvasPosition*unitWidth;
                        y = (traceboxHeight + margin - unitHeight - 2.5*unitHeight*t + vertCanvasPosition*5 - 15);
                        if (x > margin + labelWidth + traceMargin)
                        {
                            glColor3f(0.0, 0.0, 0.0);
                            glRasterPos2f(x,y);
                            for (i=0; i < axisLabel.Len() ; i++)
                            {
                                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, axisLabel[i]);
                            }
                        }
                    }
                }
            }

            // Write out labels for the traces
            for (int j=0; j<monitorNameVector.size(); j++)
            {
                y = (traceboxHeight-1 - 2.5*unitHeight*j + vertCanvasPosition*5);
                glColor3f(0.0, 0.0, 0.0);
                glRasterPos2f(margin/2,y);

                wxString traceText;
                traceText = monitorNameVector[j];

                // Write out label for a trace
                for (i = 0; i < traceText.Len() ; i++)
                {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, traceText[i]);
                }
            }
        }
    }
    else
    {
                y = (traceboxHeight-1 - 2.5*unitHeight*j + vertCanvasPosition*5);
                glColor3f(1.0, 0.0, 0.0);
                glRasterPos2f(width/10,height/2);

                wxString traceText = wxT("An Error Occured on Loading and the Canvas could not be Drawn.\nPlease open a valid .ldf file");

                // Write out label for a trace
                for (i = 0; i < traceText.Len() ; i++)
                {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, traceText[i]);
                }
    }

/*    for (int j=0; j<625; j++)
    {   
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(margin+labelWidth+traceMargin+j,500);
        glEnd();    
    }
*/    

  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
    glFlush();
    SwapBuffers();
}

void MyGLCanvas::populateTraceMatrix()
{
    for (int n=0; n < (mmz->moncount()); n++)
    {
        vector<asignal> emptyVector;
        traceMatrix.push_back(emptyVector);

        for (int i=0;i<CANVAScyclesCompleted;i++)
        {
            asignal s;
            bool ok = mmz->getsignaltrace(n, i, s);
            if (ok)
            {
                traceMatrix[n].push_back(s);
            }
            else
            {
                cout << "Error populating Matrix" << endl;
            }
        }

    }
}

void MyGLCanvas::appendToTraceMatrix()
{
    for (int n=0; n < (mmz->moncount()); n++)
    {
        for (int i=0;i<CANVAScyclesCompleted;i++)
        {
            asignal s;
            bool ok = mmz->getsignaltrace(n, i, s);
            if (ok)
            {

                traceMatrix[n].push_back(s);
            }
            else
            {
                cout << "Error appending Matrix" << endl;
            }
        }
    }
}

void MyGLCanvas::populateMonitorNameVector()
{
    monitorNameVector.clear();
    for (int n=0; n < (mmz->moncount()); n++)
    {
        name_t dev, outp;
        mmz->getmonname(n,dev,outp);
        namestring_t namestring = nmz->getName(dev);
        wxString devStr(namestring.c_str(), wxConvUTF8);
        if (outp != blankname)
        {
            namestring_t type = nmz->getName(outp);
            wxString devType(type.c_str(), wxConvUTF8);
            devStr.Append(wxT("."));
            devStr.Append(devType);
        }
        monitorNameVector.push_back(devStr);
    }
}

void MyGLCanvas::setCANVAStotalCycles(int c)
{
    CANVAStotalCycles = c;
}

void MyGLCanvas::setCANVAScyclesCompleted(int c)
{
    CANVAScyclesCompleted = c;
}



// Function to set size of vertical scroll bar, called when canvas resized
void MyGLCanvas::setCanvasVerticalScrollBar()
{
    vertCanvasPosition = 0;
  // Proportion of a trace by which the canvas will move when scroll position changes
    int scrollHeight = unitHeight/5;
  // Set maximum scroll point such that only the last monitor is visible
    int numPositions = scrollHeight * 2.5 * (monitorNameVector.size()-1);
    SetScrollbar(wxVERTICAL,0,10,10+numPositions,true);
}

// Function to set size of horizontal scroll bar, called when canvas resized
void MyGLCanvas::setCanvasHorizontalScrollBar(int position)
{   
    int width, height;
    GetClientSize(&width,&height);
    int numHorizPositions = (CANVAStotalCycles) - ((width-90)/5);        
    if (position == -1)
        position = numHorizPositions;
    if (numHorizPositions > 0)
        SetScrollbar(wxHORIZONTAL,position,100,100+numHorizPositions,true);
    else
        SetScrollbar(wxHORIZONTAL,0,100,-1,true);
}

void MyGLCanvas::setHorizontalPosition(int position)
{
    horizCanvasPosition = position;
}

void MyGLCanvas::setVerticalPosition(int position)
{
    vertCanvasPosition = position;
}

void MyGLCanvas::setNames(names* names_mod)
{
    nmz = names_mod;
}

void MyGLCanvas::setMonitor(monitor* monitor_mod)
{
    mmz = monitor_mod;
}

void MyGLCanvas::setNetwork(network* network_mod)
{
    netz = network_mod;
}

void MyGLCanvas::setClearToDrawFlag(bool flag)
{
    clearToDraw = flag;
}


// Function to initialise the GL context
void MyGLCanvas::InitGL()
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

// Callback function for when the canvas is exposed
void MyGLCanvas::OnPaint(wxPaintEvent& event)
{
    int w, h;
    wxString text;

    wxPaintDC dc(this); // required for correct refreshing under MS windows
    GetClientSize(&w, &h);
    text.Printf(wxT("Canvas redrawn by OnPaint callback, canvas size is %d by %d"), w, h);
    Render(CANVAStotalCycles);
}

// Callback for Scroll Event
void MyGLCanvas::OnScroll(wxScrollWinEvent& event)
{   
    if (event.GetOrientation() == wxVERTICAL)
    {
        vertCanvasPosition = event.GetPosition();
        Render(CANVAStotalCycles);
    }
    else if (event.GetOrientation() == wxHORIZONTAL)
    {
        horizCanvasPosition = event.GetPosition();
        Render(CANVAStotalCycles);
    }
}

// Callback function for when the canvas is resized
void MyGLCanvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event); // required on some platforms
    init = false;
    setCanvasVerticalScrollBar();
    int position = GetScrollPos(wxHORIZONTAL);
    setCanvasHorizontalScrollBar(position);
    vertCanvasPosition = 0;
    Refresh(); // required by some buggy nvidia graphics drivers,
    Update();  // harmless on other platforms!
}








// MyFrame *******************************************************************************************************************************
//         *******************************************************************************************************************************
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  // Menu Events
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
    EVT_MENU(wxID_NEW,MyFrame::OnNew)

  // Button Events
    EVT_BUTTON(RUN_BUTTON,  MyFrame::OnRunButton)
    EVT_BUTTON(CONT_BUTTON, MyFrame::OnContButton)
    EVT_BUTTON(ZAP_TRACE_BUTTON, MyFrame::OnButtonZap)
    EVT_BUTTON(ADD_TRACE_BUTTON, MyFrame::OnButtonAdd)
    EVT_BUTTON(SWITCH_BUTTON_0, MyFrame::OnButtonSwitch0)
    EVT_BUTTON(SWITCH_BUTTON_1, MyFrame::OnButtonSwitch1)
    EVT_BUTTON(START_TIMER_BUTTON, MyFrame::OnButtonStartTimer)
    EVT_BUTTON(STOP_TIMER_BUTTON, MyFrame::OnButtonStopTimer)

  // Command Line Events
    EVT_TEXT_ENTER(COMMAND_LINE, MyFrame::OnText)
    
  // Rolling Timer
    EVT_TIMER(ROLLING_TIMER, MyFrame::OnTimer)  
END_EVENT_TABLE()


MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
         names *names_mod, devices *devices_mod, monitor *monitor_mod, network *network_mod, scanner_t *scanner_mod,
         parser *parser_mod , long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
    clearToRun = true;
    SetIcon(wxIcon(wx_icon));
    nmz = names_mod;
    dmz = devices_mod;
    mmz = monitor_mod;
    netz = network_mod;
    smz = scanner_mod;
    pmz = parser_mod;

    if (nmz == NULL || dmz == NULL || mmz == NULL)
    {
        cout << "Cannot operate GUI without names, devices and monitor classes" << endl;
        exit(1);
    }

    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_NEW ,wxT("&New"));
    fileMenu->Append(wxID_OPEN ,wxT("&Open"));
    fileMenu->Append(wxID_EXIT, wxT("&Quit"));


    menuBar->Append(fileMenu, wxT("&File"));
    SetMenuBar(menuBar);
    
 //   wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL,-1,wxT("ToolBar"));
 //   toolBar->AddTool(wxID_EXIT,exit,wxT("Exit Application"));
    
    // Define the Trace Canvas
    canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, network_mod);
    FRAMEcyclesCompleted = 0;
    FRAMEtotalCycles = 0;
    runnetwork(10);
    FRAMEtotalCycles += FRAMEcyclesCompleted;
    canvas->setCANVAScyclesCompleted(FRAMEcyclesCompleted);
    canvas->setCANVAStotalCycles(FRAMEtotalCycles);
    canvas->populateMonitorNameVector();
    populateSwitchNameVector();
    
    canvas->populateTraceMatrix();
    canvas->setCanvasVerticalScrollBar();
    canvas->setCanvasHorizontalScrollBar(0);
    
    int numDevices = canvas->deviceNameVector.size();
    int numMonitors = canvas->monitorNameVector.size();
    int numSwitches = switchNameVector.size();
    int c=0;

    // Define the sizers required
    wxBoxSizer *frame_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sidebar_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer *combo_sizer = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Add or Zap Traces"));



    // Initialise combo-box strings
    wxArrayString zapTraceArray;
    wxArrayString addTraceArray;
    wxArrayString switchArray;

    // Make the zap combobox strings
    for (int i=0; i<numMonitors; i++)
    {
        zapTraceArray.Add(canvas->monitorNameVector[i],1);
    }

    // Make the add combobox strings
    for (int i=0; i<numDevices; i++)
    {
        wxString deviceNamestring = canvas->deviceNameVector[i];
        name_t did = getIdFromWxString(deviceNamestring);
        devicekind dkind = netz->netzdevkind(did);

        if (dkind == dtype)
        {
            addTraceArray.Add(deviceNamestring.Append(wxT(".Q")));
            c++;
            addTraceArray.Add(deviceNamestring.Append(wxT("BAR")));
            c++;
        }
        else
        {
            addTraceArray.Add(deviceNamestring);
            c++;
        }
    }

    // Make the switch combo-box strings
    for (int i=0; i<numSwitches; i++)
    {
        switchArray.Add(switchNameVector[i]);
    }





  // DEFINITIONS OF BUTTONS, CONTROLS AND SIZERS
  // *********************************************************************************************************************************

  //Define Zap Controls
    zapTraceComboBox = new wxComboBox(this,ZAP_TRACE_COMBO_BOX, wxT("Choose trace to zap"),wxDefaultPosition, wxDefaultSize,zapTraceArray);
    zapTraceButton = new wxButton(this, ZAP_TRACE_BUTTON, wxT("ZAP"));

  // Define Add Controls
    addTraceComboBox = new wxComboBox(this,ADD_TRACE_COMBO_BOX, wxT("Choose trace to add"),wxDefaultPosition, wxDefaultSize,addTraceArray);
    addTraceButton = new wxButton(this, ADD_TRACE_BUTTON, wxT("ADD"));

  // Define Switch Controls
    switchComboBox = new wxComboBox(this,SWITCH_COMBO_BOX, wxT("Choose Switch "), wxDefaultPosition, wxDefaultSize, switchArray);
    switchButton0 = new wxButton(this, SWITCH_BUTTON_0, wxT("Set to 0"));
    switchButton1 = new wxButton(this, SWITCH_BUTTON_1, wxT("Set to 1"));

  // Define Other buttons
    runButton = new wxButton(this, RUN_BUTTON, wxT("Run"));
    contButton = new wxButton(this, CONT_BUTTON, wxT("Continue"));
    runSpin = new wxSpinCtrl(this, RUN_SPINCTRL, wxString(wxT("10")));
    contSpin = new wxSpinCtrl(this, CONT_SPINCTRL, wxString(wxT("10")));
    commandLine = new wxTextCtrl(this, COMMAND_LINE, wxT(""), wxDefaultPosition, wxSize(150,60), wxTE_PROCESS_ENTER|wxTE_MULTILINE);
    commandLine->WriteText(wxT("# GUI running. Press Run to display signals."));
    commandLine->SetInsertionPoint(0);
    runStaticText = new wxStaticText(this, wxID_ANY, wxT("Cycles"));
    contStaticText = new wxStaticText(this, wxID_ANY, wxT("Cycles"));
    startTimerButton = new wxButton(this, START_TIMER_BUTTON, wxT("Run Continuously"));
    stopTimerButton = new wxButton(this, STOP_TIMER_BUTTON, wxT("Stop Running"));
    
  // Define the Timer
    rollingTimer = new wxTimer(this, ROLLING_TIMER);

  // Place Zap Controls
    wxBoxSizer *zap_sizer = new wxBoxSizer(wxVERTICAL);
    zap_sizer->Add(zapTraceComboBox,0,wxTOP|wxLEFT,5);
    zap_sizer->Add(zapTraceButton,0,wxALIGN_CENTRE|wxBOTTOM,20);

  // Place Add controls
    wxBoxSizer *add_sizer = new wxBoxSizer(wxVERTICAL);
    add_sizer->Add(addTraceComboBox,0,wxTOP|wxRIGHT,5);
    add_sizer->Add(addTraceButton,0,wxALIGN_CENTRE|wxBOTTOM,20);

  // Place switch combo
    wxStaticBoxSizer *switch_sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Change Switches"));
    switch_sizer->Add(switchComboBox,0,wxALIGN_CENTRE | wxEXPAND|wxLEFT|wxRIGHT|wxTOP,20);

  // Place switch buttons
    wxBoxSizer *switchButton_sizer = new wxBoxSizer(wxHORIZONTAL);
    switchButton_sizer->Add(switchButton0,0,wxLEFT|wxBOTTOM,20);
    switchButton_sizer->Add(switchButton1,0,wxRIGHT|wxBOTTOM,20);

  // Place run Buttons
    wxBoxSizer *run_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    run_button_sizer->Add(runButton, 0, wxTOP|wxLEFT, 20);
    run_button_sizer->Add(runSpin, 0, wxEXPAND | wxTOP, 20);
    run_button_sizer->Add(runStaticText,0,wxALIGN_CENTRE|wxTOP|wxRIGHT,20);

  // Place run Buttons
    wxBoxSizer *cont_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    cont_button_sizer->Add(contButton, 0, wxBOTTOM|wxLEFT, 20);
    cont_button_sizer->Add(contSpin, 0, wxEXPAND | wxBOTTOM, 20);
    cont_button_sizer->Add(contStaticText,0,wxALIGN_CENTRE|wxBOTTOM|wxRIGHT,20);
    
  // Network Static Box Sizer   
    wxStaticBoxSizer *network_sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Run or Continue Network"));
    network_sizer->Add(run_button_sizer,0,wxALIGN_CENTRE,0);
    network_sizer->Add(cont_button_sizer,0,wxALIGN_CENTRE,0);
    
  // Place timer Buttons
    wxStaticBoxSizer *timer_button_sizer = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Continuous Mode"));
    timer_button_sizer->Add(startTimerButton,0,wxALIGN_CENTRE|wxALL,20);
    timer_button_sizer->Add(stopTimerButton,0,wxALIGN_CENTRE| wxALL,20);


  // *********************************************************************************************************************************




  // NESTED SIZERS FOR PLACEMENT OF CONTROL LOOPS
  //**********************************************************************************************************************************
    frame_sizer->Add(topsizer,5, wxEXPAND | wxTOP|wxLEFT|wxBOTTOM,20);
    frame_sizer->Add(commandLine,1,wxEXPAND | wxALL,10);

    topsizer->Add(canvas,3,wxEXPAND,0);
    topsizer->Add(sidebar_sizer,2,wxALIGN_CENTRE,0);

    sidebar_sizer->Add(combo_sizer,0,wxALIGN_CENTRE|wxALL, 20);
    sidebar_sizer->Add(switch_sizer, 0, wxALIGN_CENTRE|wxALL, 20);
    sidebar_sizer->Add(network_sizer,0,wxALIGN_CENTRE,0);
    sidebar_sizer->Add(timer_button_sizer,0,wxALIGN_CENTRE|wxALL, 20);

    combo_sizer->Add(zap_sizer, 0, wxALIGN_CENTER);
    combo_sizer->Add(add_sizer, 0, wxALIGN_CENTRE);

    switch_sizer->Add(switchButton_sizer,0,wxALIGN_CENTRE);
//************************************************************************************************************************************

    SetSizeHints(1000, 400);
    SetSizer(frame_sizer);
}


void MyFrame::OnExit(wxCommandEvent &event)
  // Callback for the exit menu item
{
    Close(true);
}

void MyFrame::OnOpen(wxCommandEvent &event)
{
    OpenFile();
}

void MyFrame::OnNew(wxCommandEvent &event)
{
    string command = "gedit untitled.ldf &";
    system(command.c_str());
}

void MyFrame::OnRunButton(wxCommandEvent &event)
{
  // Call the run function to run the network for the number of cycles currently in the combo box
  if (clearToRun)
  {
      RunFunction();
  }
}


void MyFrame::OnContButton(wxCommandEvent &event)
{
    if (clearToRun)
    {
        ContinueFunction();
    }
}


  // Callback for Zap PushButton
void MyFrame::OnButtonZap(wxCommandEvent &event)
{
    if (clearToRun)
    {
     // Take values from zap combobox
        wxString selectionStr = zapTraceComboBox->GetStringSelection();
        int selection = zapTraceComboBox->GetSelection();
        if (selectionStr.IsEmpty())
        {
            errorBox(wxT("You need to select a device to monitor!"));
            zapTraceComboBox->SetValue(wxT("Choose trace to zap"));
            return;
        }

     // Error checking
        bool found = false;
        for (int i=0; i<canvas->monitorNameVector.size(); i++)
        {
            if (selectionStr == canvas->monitorNameVector[i])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            errorBox(wxT("Sorry we couldn't find the monitor you tried to zap"));
            return;
        }
      // Find namestring of device
        string devNamestring = string(selectionStr.mb_str());
        wxString commandLineText;
        commandLineText.Printf(wxT("# %s removed\n"),selectionStr.c_str());

      // Remove the selected monitor from the monitorNameVector and delete selection from combobox list
        canvas->monitorNameVector.erase(canvas->monitorNameVector.begin() + selection);
        zapTraceComboBox->Delete(selection);
        // Get monintor name and remove it from the list of monitors in mmz
        name_t did, outp;
        mmz->getmonname(selection, did, outp);
        bool ok;
        mmz->remmonitor(did, outp, ok);
        if (!ok)
        {
            cout << "Something went wrong with removing a monitor" << endl;
        }
     // Populate the new traceMatrix and Render the canvas to remove the desired trace
        canvas->traceMatrix.erase(canvas->traceMatrix.begin() + selection);
        WriteToCommandLine(commandLineText);
     // Reset the text in the ComboBox
        zapTraceComboBox->SetValue(wxT("Choose trace to zap"));
     // Resize the scroll bar
        canvas->setCanvasVerticalScrollBar();
        canvas->Render(FRAMEtotalCycles);
    }
}

void MyFrame::OnButtonAdd(wxCommandEvent &event)
  // Callback for second pushbutton
{  // Get string selected and index of selection
    if (clearToRun)
    {

        wxString selectionStr = addTraceComboBox->GetStringSelection();
        int selection = addTraceComboBox->GetSelection();

        if (selectionStr.IsEmpty())
        {
            errorBox(wxT("You need to select a device to monitor"));
            addTraceComboBox->SetValue(wxT("Choose trace to add"));
            return;
        }
        bool isDtype;
        wxString deviceName;
        wxString outputName;
        wxString monitorName = selectionStr;
        checkMonitorName(monitorName,deviceName,outputName,isDtype);

      // Convert chosen string into a nameString
        string deviceString = string(deviceName.mb_str());
        namestring_t namestring = (namestring_t) deviceString;

      // Add chosen monitor to list of monitors in mmz
        name_t did, outp;
        did = nmz->cvtname(namestring);
        if (isDtype)
        {
            namestring_t type = string(outputName.mb_str());
            outp = nmz->cvtname(type);
        }
        else
            outp = blankname;

        bool ok;
        mmz->makemonitor(did, outp, ok);
        if (!ok)
        {
            cout << "Something went wrong with adding a monitor" << endl;
        }

      // Add chosen monitor to monitorNameVector, append monitor to zap button.
        canvas->monitorNameVector.push_back(selectionStr);
        zapTraceComboBox->Append(selectionStr);

      // Create message string for the canvas
        wxString text;
        text.Printf(wxT("%s added"),selectionStr.c_str());

      // Warn the user that I'll run for 10 cycles
        wxString commandLineText;
        commandLineText.Printf(wxT("# %s added. Network will now be run for %d cycles\n"),selectionStr.c_str(),runSpin->GetValue());
        WriteToCommandLine(commandLineText);

      // Run the network for a few values
        RunFunction();

      // Reset the text in the ComboBox
        addTraceComboBox->SetValue(wxT("Choose trace to add"));

      // Resize the scroll bar
        canvas->setCanvasVerticalScrollBar();
    }
}

// Callback for second pushbutton
void MyFrame::OnButtonSwitch0(wxCommandEvent &event)
{
    if (clearToRun)
    {
        if (switchComboBox->GetStringSelection().IsEmpty())
        {
            errorBox(wxT("You need to select a device to monitor"));
            addTraceComboBox->SetValue(wxT("Choose trace to add!"));
            return;
        }

        name_t sid = getIdFromWxString(switchComboBox->GetStringSelection());
        asignal s = low;
        bool ok;

        dmz->setswitch(sid, s, ok);
        if (!ok)
            cout << "Error setting switch to 0" << endl;

        wxString swStr = switchComboBox->GetStringSelection();
        wxString commandLineText;
        commandLineText.Printf(wxT("# %s changed to 0\n"),swStr.c_str());
        WriteToCommandLine(commandLineText);
    }
}

// Callback for second pushbutton
void MyFrame::OnButtonSwitch1(wxCommandEvent &event)
{   
    if (clearToRun)
    {

        if (switchComboBox->GetStringSelection().IsEmpty())
        {
            errorBox(wxT("You need to select a device to monitor"));
            addTraceComboBox->SetValue(wxT("Choose trace to add!"));
            return;
        }

        name_t sid = getIdFromWxString(switchComboBox->GetStringSelection());
        asignal s = high;
        bool ok;

        dmz->setswitch(sid, s, ok);
        if (!ok)
            cout << "Error setting switch to 1" << endl;

        wxString swStr = switchComboBox->GetStringSelection();
        wxString commandLineText;
        commandLineText.Printf(wxT("# %s changed to 1\n"),swStr.c_str());
        WriteToCommandLine(commandLineText);
    }
}

  // Callback for the text entry field
void MyFrame::OnText(wxCommandEvent &event)
{
    if (clearToRun)
    {
        int numberOfLines = commandLine->GetNumberOfLines();
        wxString text;
        text.Printf(wxT("cmd: %s"), commandLine->GetLineText(numberOfLines).c_str());
        commandLine->SetInsertionPoint(0);
        commandLine->WriteText(wxT("\n"));
        commandLine->SetInsertionPoint(0);
        commandLine->WriteText(wxT("# "));
        commandLine->SetInsertionPoint(2);
    }
}

  // Callback for the time button
void MyFrame::OnButtonStartTimer(wxCommandEvent &event)
{
    if (clearToRun)
    {
       int INTERVAL = 500; // milliseconds
       rollingTimer->Start(INTERVAL);
    }
}

void MyFrame::OnButtonStopTimer(wxCommandEvent &event)
{
    if (clearToRun)
    {
        rollingTimer->Stop();
    }
}


void MyFrame::OnTimer(wxTimerEvent &event)
{
    if (clearToRun)
    {
        ContinueFunction();
    }
}


void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
    FRAMEcyclesCompleted = 0;
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
        {
            cout << "Error: network is oscillating" << endl;
        }
    }

    if (ok)
    {
        FRAMEcyclesCompleted += ncycles;
    }
    else
    {
        FRAMEcyclesCompleted = 0;
    }
}

void MyFrame::RunFunction()
{
    int ncycles;
    FRAMEcyclesCompleted = 0;
    FRAMEtotalCycles = 0;
    canvas->setHorizontalPosition(0);
    ncycles = runSpin->GetValue();


  // Clear the traceMatrix
    if (canvas->traceMatrix.size() >0)
    {
        canvas->traceMatrix.clear();
    }

  // Reset the monitor and run network
    mmz->resetmonitor();
    runnetwork(ncycles);
    FRAMEtotalCycles += FRAMEcyclesCompleted;
    canvas->setCANVAScyclesCompleted(FRAMEcyclesCompleted);
    canvas->setCANVAStotalCycles(FRAMEtotalCycles);

  // Populate the traceMatrix and render the canvas
    canvas->populateTraceMatrix();
    canvas->Render(FRAMEtotalCycles);
    canvas->setCanvasHorizontalScrollBar(0);
    wxString commandLineText;
    commandLineText.Printf(wxT("# Network run for %d cycles\n"),ncycles);
    WriteToCommandLine(commandLineText);
}


void MyFrame::ContinueFunction()
{   
    FRAMEcyclesCompleted = 0;
    int width, height;
    canvas->GetClientSize(&width,&height);

    int maxcycles = 100000;
    if (FRAMEtotalCycles > maxcycles)
    {
        errorBox(wxT("Can't display any more cycles. Click ""Run"" to start again. "));
        return;
    }

  // Reset the monitor and run network
    mmz->resetmonitor ();
    runnetwork(contSpin->GetValue());
    FRAMEtotalCycles += FRAMEcyclesCompleted;
    
    canvas->setCANVAScyclesCompleted(FRAMEcyclesCompleted);
    canvas->setCANVAStotalCycles(FRAMEtotalCycles);

  // Populate the traceMatrix and render the canvas
    canvas->appendToTraceMatrix();
    int canvasHeight, canvasWidth;
    canvas->GetClientSize(&canvasWidth, &canvasHeight);
    
    if (FRAMEtotalCycles > ((canvasWidth-90)/5))
    {   
        canvas->setHorizontalPosition(FRAMEtotalCycles-((canvasWidth-90)/5));
        canvas->setCanvasHorizontalScrollBar(-1);
    }
    canvas->Render(FRAMEtotalCycles);
    
    
    wxString commandLineText;
    commandLineText.Printf(wxT("# Network continued for %d cycles\n"),FRAMEcyclesCompleted);
    WriteToCommandLine(commandLineText);
    
}

void MyFrame::errorBox(wxString errorBox)
{
    wxString message;
    message.Printf(wxT("%s"),errorBox.c_str());
    wxMessageDialog about(this,message,wxT("About"), wxICON_INFORMATION | wxOK);
    about.ShowModal();
    return;
}

bool MyFrame::isdtype(name_t did)
{
    devicekind dkind = netz->netzdevkind(did);
    if (dkind == dtype)
        return true;
    else
        return false;
}

void MyFrame::checkMonitorName(wxString monitorName, wxString& deviceName, wxString& outputName, bool& isDtype)
{
    //int index = deviceName.Find('.');
    deviceName = monitorName.BeforeFirst('.');
    outputName = monitorName.AfterFirst('.');
    if (outputName.IsEmpty())
        isDtype = false;
    else
        isDtype = true;
}

void MyFrame::resetCanvas()
{
  // Clear the old Canvas data
    canvas->traceMatrix.clear();
    canvas->deviceNameVector.clear();
    canvas->monitorNameVector.clear();

  // Reset the names, monitor and network classes/
    canvas->setNames(nmz);
    canvas->setMonitor(mmz);
    canvas->setNetwork(netz);

    canvas->numDtypes=0;

  // Populate deviceNameVector with the wxString names of all devices in the network
    devlink dlink = netz->devicelist();     // Find beginning of the list of devices
    while (dlink != NULL)
    {
        name_t did = dlink->id;
        namestring_t namestring = nmz->getName(did);
        devicekind dkind = netz->netzdevkind(did);
        if (dkind == dtype)
            canvas->numDtypes++;
        wxString devStr(namestring.c_str(), wxConvUTF8);
        canvas->deviceNameVector.push_back(devStr);
        dlink = dlink->next;
    }

  // Reset the network and run for a few cycles (set to an arbitrary 10 here)
    FRAMEtotalCycles = 0;
    FRAMEcyclesCompleted = 0;
    canvas->setCANVAStotalCycles(0);
    canvas->setCANVAScyclesCompleted(0);
    canvas->setHorizontalPosition(0);
    canvas->setVerticalPosition(0);
    
  // Run network for 10 cycles
    mmz->resetmonitor();   
    runnetwork(10);
  
  // Update all the cycles values  
    FRAMEtotalCycles += FRAMEcyclesCompleted;
    canvas->setCANVAStotalCycles(FRAMEtotalCycles);
    canvas->setCANVAScyclesCompleted(FRAMEcyclesCompleted);

  // Populate the traceMatrix, switchVector, monitorNameVector
    canvas->populateMonitorNameVector();
    populateSwitchNameVector();
    canvas->populateTraceMatrix();
    
  // Set the scroll bars   
    canvas->setCanvasVerticalScrollBar();
    canvas->setCanvasHorizontalScrollBar(0);

  // Initialise values for populating the comboBox
    int numDevices = canvas->deviceNameVector.size();
    int numMonitors = canvas->monitorNameVector.size();
    int numSwitches = switchNameVector.size();
    int c=0;

  // Initialise comboBox strings
    wxArrayString zapTraceArray;
    wxArrayString addTraceArray;
    wxArrayString switchArray;

  // Make the zap comboBox strings
    for (int i=0; i<numMonitors; i++)
    {
        zapTraceArray.Add(canvas->monitorNameVector[i],1);
    }

  // Make the add comboBox strings
    for (int i=0; i<numDevices; i++)
    {
        wxString deviceNamestring = canvas->deviceNameVector[i];
        name_t did = getIdFromWxString(deviceNamestring);
        devicekind dkind = netz->netzdevkind(did);

        if (dkind == dtype)
        {
            addTraceArray.Add(deviceNamestring.Append(wxT(".Q")));
            c++;
            addTraceArray.Add(deviceNamestring.Append(wxT("BAR")));
            c++;
        }
        else
        {
            addTraceArray.Add(deviceNamestring);
            c++;
        }
    }

  // Make the switch comboBox strings
    for (int i=0; i<numSwitches; i++)
    {
        switchArray.Add(switchNameVector[i]);
    }


  // Reset the Zap, Add and Switch comboBox selection Lists
    zapTraceComboBox->Clear();
    zapTraceComboBox->Append(zapTraceArray);


    addTraceComboBox->Clear();
    addTraceComboBox->Append(addTraceArray);

    switchComboBox->Clear();
    switchComboBox->Append(switchArray);
    
}

void MyFrame::OpenFile()
{
  // To start with, stop the rolling timer.
    rollingTimer->Stop();
    
    wxFileDialog *openDialog = new wxFileDialog(this,wxT("Open file"), wxT(""), wxT(""), wxT("*.ldf"),wxFD_OPEN, wxDefaultPosition);
    openDialog->ShowModal();
    
    
    
    wxString fid = openDialog->GetPath();
    wxString fileName = fid.AfterLast('/');
    string fileid = string(fid.mb_str());

    delete nmz;
    delete netz;
    delete dmz;
    delete mmz;
    delete smz;
    delete pmz;

    nmz = new names();
    netz = new network(nmz);
    dmz = new devices(nmz, netz);
    mmz = new monitor(nmz, netz);
    smz = new scanner_t(fileid);

    pmz = new parser(netz, dmz, mmz, smz, nmz);
    symbol_t symbol;
    symbol.symboltype = startfsym;      // arbitrary symboltype that is not eofsym


    /* Read through whole file outputting one symbol and its type at a time */
    while (symbol.symboltype != eofsym)
    {
        smz->nextSymbol(symbol);
        pmz->readin(symbol);

    }
    vector<Error*> errorVector = pmz->getErrorVector();
    vector<Warning*> warningVector = pmz->getWarningVector();

    int line, col;
    string errorMessage;
    bool hasPosition;
    
    for (int i=0; i<errorVector.size(); i++)
    {
        errorVector[i]->getErrorDetails(line, col, errorMessage, hasPosition);
        string errorString = smz->printError(line, col, errorMessage, hasPosition);
            
        wxString wxErrorString(errorString.c_str(), wxConvUTF8);
        WriteToCommandLine(wxErrorString);
    }
    
    for (int i=0; i<warningVector.size(); i++)
    {
        warningVector[i]->getWarningDetails(line, col, errorMessage, hasPosition);
        string warningString = smz->printError(line, col, errorMessage, hasPosition);
        
        wxString wxWarningString(warningString.c_str(), wxConvUTF8);
        WriteToCommandLine(wxWarningString);
        
    }
    
    if (errorVector.size() != 0)
    {
//        WriteToCommandLine(commandLineText);
        canvas->setClearToDrawFlag(false);
        clearToRun = false;
        canvas->Render(0);
        return;
    }
    
    canvas->setClearToDrawFlag(true);
    clearToRun = true;
    resetCanvas();
    canvas->Render(FRAMEtotalCycles);
    wxString commandLineText;
    commandLineText.Printf(wxT("# '%s' opened, Network run for a few cycles\n"),fileName.c_str());
    WriteToCommandLine(commandLineText);
}

void MyFrame::WriteToCommandLine(wxString commandLineText)
{
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
}

void MyFrame::populateSwitchNameVector()
{
    switchNameVector.clear();
    for (int i=0; i<(canvas->deviceNameVector.size()); i++)
    {
        // Convert wxString in the vector to namestring_t (std::string)
        name_t did = getIdFromWxString(canvas->deviceNameVector[i]);
        devicekind dkind = netz->netzdevkind(did);
        if (dkind == aswitch)
        {
            switchNameVector.push_back(canvas->deviceNameVector[i]);
        }
    }
}


name_t MyFrame::getIdFromWxString(wxString inStr)
{
    namestring_t devStr = string(inStr.mb_str());
    return nmz->cvtname(devStr);
}




