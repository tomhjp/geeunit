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
    vertCanvasPosition = 0;
    horizCanvasPosition = 0;
    unitHeight = 20;
    numDtypes = 0;

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

void MyGLCanvas::Render(wxString example_text, int cycles)
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

    if (cycles > 0)
        cyclesdisplayed = cycles;

//    unitWidth = traceboxWidth / cyclesdisplayed;

//    if (unitWidth < 2)
        unitWidth = 5;

    SetCurrent();

    if (!init)
    {
        InitGL();
        init = true;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    //If there are monitors then draw the first monitor signal, get trace from monitor class
    if ((cyclesdisplayed > 0) && (mmz->moncount() > 0))
    {

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
                        x = margin + labelWidth + traceMargin + unitWidth*c - horizCanvasPosition*5;
                    }

                    if (s==high)
                    {
                        y = (traceboxHeight + margin - 2.5*unitHeight*t + vertCanvasPosition*5);
                        x = margin + labelWidth + traceMargin + unitWidth*c - horizCanvasPosition*5;
                    }
                    
                    if (x > margin + labelWidth + traceMargin)
                        glVertex2f(x,y);
                    if (x + unitWidth > margin + labelWidth + traceMargin)
                        glVertex2f(x+unitWidth, y);
                    
                }
            glEnd();
        }

        // WHY IS THERE NO ROUND() FUNCTION?!?!
        int axisSpacing = 5;
        if (cyclesdisplayed < 50)
            axisSpacing = 2;
        else if (cyclesdisplayed < 100)
            axisSpacing = 5;
        else if (cyclesdisplayed < 200)
            axisSpacing = 10;
        else if (cyclesdisplayed < 400)
            axisSpacing = 20;
        else
            axisSpacing = 100;


        for (t=0; t<traceMatrix.size(); t++)
        {
            for (c=0; c<traceMatrix[0].size(); c++)
            {
                if (c%axisSpacing == 0)
                {
                    wxString axisLabel;
                    axisLabel.Printf(wxT("%d"),c);
                    x = margin + labelWidth + traceMargin + unitWidth*c -6 - horizCanvasPosition*5;
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

    

  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
    glFlush();
    SwapBuffers();
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
    Render(text,-1);
}

// Callback for Scroll Event
void MyGLCanvas::OnScroll(wxScrollWinEvent& event)
{   
    if (event.GetOrientation() == wxVERTICAL)
    {
        vertCanvasPosition = event.GetPosition();
        Render(wxT("Scrolling"),-1);
    }
    else if (event.GetOrientation() == wxHORIZONTAL)
    {
        horizCanvasPosition = event.GetPosition();
        Render(wxT("Scrolling"),-1);
    }
}

// Function to set size of scroll bar, called when canvas resized
void MyGLCanvas::setCanvasVerticalScrollBar()
{
    vertCanvasPosition = 0;
    // Proportion of a trace by which the canvas will move when scroll position changes
    int scrollHeight = unitHeight/5;
    // Set maximum scroll point such that only the last monitor ihorizCanvasPosition*5s visible
    int numPositions = scrollHeight * 2.5 * (monitorNameVector.size()-1);
    SetScrollbar(wxVERTICAL,0,10,10+numPositions);
}

void MyGLCanvas::setCanvasHorizontalScrollBar()
{   
    int scrollWidth = unitWidth;
    int numPositions = (cyclesdisplayed - 50);
    cout << cyclesdisplayed << endl;
    if (numPositions > 0)
        SetScrollbar(wxHORIZONTAL,0,100,100+numPositions);
    else
        cout << "negative" << endl;
}


// Callback function for when the canvas is resized
void MyGLCanvas::OnSize(wxSizeEvent& event)
{
    wxGLCanvas::OnSize(event); // required on some platforms
    init = false;
    setCanvasVerticalScrollBar();
    setCanvasHorizontalScrollBar();
    vertCanvasPosition = 0;
    horizCanvasPosition = 0;
    Refresh(); // required by some buggy nvidia graphics drivers,
    Update();  // harmless on other platforms!
}

  // Callback function for mouse events inside the GL canvas
void MyGLCanvas::OnMouse(wxMouseEvent& event)
{
    // Currently not doing anything
    return;
}

void MyGLCanvas::populateTraceMatrix()
{
    for (int n=0; n < (mmz->moncount()); n++)
    {
        vector<asignal> emptyVector;
        traceMatrix.push_back(emptyVector);

        for (int i=0;i<cyclescompleted;i++)
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
        for (int i=0;i<cyclescompleted;i++)
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


void MyGLCanvas::setCyclesDisplayed(int c)
{
    cyclesdisplayed = c;
}

void MyGLCanvas::setCyclesCompleted(int c)
{
    cyclescompleted = c;
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

  // Combo Box Events
    EVT_COMBOBOX(ZAP_TRACE_COMBO_BOX, MyFrame::OnSelect)
    EVT_COMBOBOX(ADD_TRACE_COMBO_BOX, MyFrame::OnSelect)

  // SpinControl Events
    EVT_SPINCTRL(RUN_SPINCTRL,  MyFrame::OnSpin)
    EVT_SPINCTRL(CONT_SPINCTRL, MyFrame::OnSpin)

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



    SetMenuBar(menuBar);;
    // Define the Trace Canvas
    canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, network_mod);
    cyclescompleted = 0;
    runnetwork(10);
    canvas->populateMonitorNameVector();
    populateSwitchNameVector();
    canvas->setCyclesCompleted(cyclescompleted);
    canvas->populateTraceMatrix();
    canvas->setCanvasVerticalScrollBar();
    canvas->setCanvasHorizontalScrollBar();
    
    int numDevices = canvas->deviceNameVector.size();
    int numMonitors = canvas->monitorNameVector.size();
    int numSwitches = switchNameVector.size();
    int c=0;

    // Define the sizers required
    wxBoxSizer *frame_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sidebar_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *combo_sizer = new wxBoxSizer(wxHORIZONTAL);



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
    commandLine = new wxTextCtrl(this, COMMAND_LINE, wxT(""), wxDefaultPosition, wxSize(150,30), wxTE_PROCESS_ENTER|wxTE_MULTILINE);
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
    switchButton_sizer->Add(switchButton0,0,0,0);
    switchButton_sizer->Add(switchButton1,0,0,0);

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
    
  // Place Buttons
    wxBoxSizer *timer_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    timer_button_sizer->Add(startTimerButton,0,wxALIGN_CENTRE|wxTOP,30);
    timer_button_sizer->Add(stopTimerButton,0,wxALIGN_CENTRE|wxTOP,30);


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
    sidebar_sizer->Add(timer_button_sizer,0,wxALIGN_CENTRE);

    combo_sizer->Add(zap_sizer, 0, wxALIGN_CENTER);
    combo_sizer->Add(add_sizer, 0, wxALIGN_CENTRE);

    switch_sizer->Add(switchButton_sizer,0,wxALIGN_CENTRE);
//************************************************************************************************************************************

    SetSizeHints(1000, 400);
    SetSizer(frame_sizer);

//    canvas->Render(wxT("INITIALISING WINDOW"),10);
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


// Callback for the run button
void MyFrame::OnRunButton(wxCommandEvent &event)
{
  // Call the run function to run the network for the number of cycles currently in the combo box
    RunFunction();
}

// Callback for the continue button
void MyFrame::OnContButton(wxCommandEvent &event)
{
    ContinueFunction();
}


  // Callback for Zap PushButton
void MyFrame::OnButtonZap(wxCommandEvent &event)
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
    
    canvas->Render(commandLineText,-1);
}

void MyFrame::OnButtonAdd(wxCommandEvent &event)
  // Callback for second pushbutton
{  // Get string selected and index of selection
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
        cout << "IT'S A DTYPE" << endl;
        namestring_t type = string(outputName.mb_str());
        cout << type << endl;
        outp = nmz->cvtname(type);
        cout << "outp = " << outp << endl;
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

// Callback for second pushbutton
void MyFrame::OnButtonSwitch0(wxCommandEvent &event)
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

// Callback for second pushbutton
void MyFrame::OnButtonSwitch1(wxCommandEvent &event)
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

  // Callback for the spin control
void MyFrame::OnSpin(wxSpinEvent &event)
{
    wxString text;
    text.Printf(wxT("New spinctrl value %d"), event.GetPosition());
    canvas->Render(text,-1);
}

  // Callback for the text entry field
void MyFrame::OnText(wxCommandEvent &event)
{
    int numberOfLines = commandLine->GetNumberOfLines();
    wxString text;
    text.Printf(wxT("cmd: %s"), commandLine->GetLineText(numberOfLines).c_str());
    commandLine->SetInsertionPoint(0);
    commandLine->WriteText(wxT("\n"));
    commandLine->SetInsertionPoint(0);
    commandLine->WriteText(wxT("# "));
    commandLine->SetInsertionPoint(2);
    canvas->Render(text,-1);
}

  // Callback for the time button
void MyFrame::OnButtonStartTimer(wxCommandEvent &event)
{
   int INTERVAL = 500; // milliseconds
   rollingTimer->Start(INTERVAL);
}

void MyFrame::OnButtonStopTimer(wxCommandEvent &event)
{
   rollingTimer->Stop();
}


void MyFrame::OnTimer(wxTimerEvent &event)
{
    ContinueFunction();
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
        {
            cout << "Error: network is oscillating" << endl;
        }
    }

    if (ok)
    {
        cyclescompleted = cyclescompleted + ncycles;
    }
    else
    {
        cyclescompleted = 0;
    }
}


void MyFrame::errorBox(wxString errorBox)
{
    wxString message;
    message.Printf(wxT("%s"),errorBox.c_str());
    wxMessageDialog about(this,message,wxT("About"), wxICON_INFORMATION | wxOK);
    about.ShowModal();
    return;
}


void MyFrame::OnSelect(wxCommandEvent &event)
{
    wxString trace = event.GetString();
}

void MyFrame::RunFunction()
{
    int  ncycles;
    cyclescompleted = 0;
    ncycles = runSpin->GetValue();


  // Clear the traceMatrix
    if (canvas->traceMatrix.size() >0)
    {
        canvas->traceMatrix.clear();
    }

  // Reset the monitor and run network
    mmz->resetmonitor();
    runnetwork(ncycles);

  // Populate the traceMatrix and render the canvas
    canvas->setCyclesCompleted(cyclescompleted);
    canvas->populateTraceMatrix();
    canvas->Render(wxT("Run button pressed"), cyclescompleted);
    wxString commandLineText;
    commandLineText.Printf(wxT("# Network run for %d cycles\n"),ncycles);
    WriteToCommandLine(commandLineText);
}


void MyFrame::ContinueFunction()
{   
    int width, height;
    canvas->GetClientSize(&width,&height);

    int maxcycles = 100000;
    if (cyclescompleted > maxcycles)
    {
        errorBox(wxT("Can't display any more cycles. Click ""Run"" to start again. "));
        return;
    }
    int newcycles = 0;

  // Reset the monitor and run network
    mmz->resetmonitor ();
    runnetwork(contSpin->GetValue());
    newcycles = contSpin->GetValue();

  // Populate the traceMatrix and render the canvas
    canvas->setCyclesCompleted(newcycles);
    canvas->appendToTraceMatrix();
    canvas->setCanvasHorizontalScrollBar();
    canvas->Render(wxT("Run button pressed"),cyclescompleted);
    wxString commandLineText;
    commandLineText.Printf(wxT("# Network continued for %d cycles\n"),newcycles);
    WriteToCommandLine(commandLineText);
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
    cyclescompleted = 0;
    runnetwork(10);

  // Populate the traceMatrix, switchVector
    canvas->populateMonitorNameVector();
    populateSwitchNameVector();
    canvas->setCyclesCompleted(cyclescompleted);
    canvas->populateTraceMatrix();
    canvas->setCanvasVerticalScrollBar();
    canvas->setCanvasHorizontalScrollBar();

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
        smz->printError(line, col, errorMessage, hasPosition);
    }
    for (int i=0; i<warningVector.size(); i++)
    {
        //warningVector[i]->getWarningDetails(line, col, errorMessage);
        //smz->printError(line, col, errorMessage);
    }
    if (errorVector.size() != 0)
    {
        wxString wxCommand;
        wxCommand.Printf(wxT("gedit %s &"),fid.c_str());
  
        string command = string(wxCommand.mb_str());
        system(command.c_str());
        wxString commandLineText;
        commandLineText.Printf(wxT("# %s contained an Error and cannot be simulated. Please open a valid file\n"),fileName.c_str());
        WriteToCommandLine(commandLineText);
        errorBox(commandLineText);
        OpenFile();
        return;
    }
    resetCanvas();
    canvas->Render(wxT("New File Opened Network run for a few cycles"),cyclescompleted);
    wxString commandLineText;
    commandLineText.Printf(wxT("# '%s' opened, Network run for a few cycles\n"),fileName.c_str());
    WriteToCommandLine(commandLineText);
}

void MyFrame::WriteToCommandLine(wxString commandLineText)
{
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
}

