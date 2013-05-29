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
    canvasPosition = 0;
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
    float y;
    unsigned int i,j,c,t;
    asignal s;
    int width, height,traceboxWidth,traceboxHeight;
    int numTraces = 10;

    int lenTrace = 20;
    int margin = 20;
    float unitWidth;
    int x;
    int labelWidth = 30;
    GetClientSize(&width,&height);
    //cout << "Width: " << width << endl;
    //cout << "Height: " << height << endl;

    traceboxWidth = width - 2*margin - labelWidth;
    traceboxHeight = height - 2*margin;
    int num = 1;


    if (cycles > 0)
        cyclesdisplayed = cycles;

    SetCurrent();

    if (!init)
    {
        InitGL();
        init = true;
    }


    glClear(GL_COLOR_BUFFER_BIT);
    
    unitWidth = traceboxWidth / cyclesdisplayed;
    if (unitWidth < 1)
        unitWidth = 0.3;
        
    cout << unitWidth << endl;  
    cout << cyclesdisplayed << endl;
//2700
    //If there are monitors then draw the first monitor signal, get trace from monitor class
    if ((cyclesdisplayed > 0) && (mmz->moncount() > 0))
    {
        
        if (unitWidth > 30)
        {
            unitWidth = 30;
        }

        for (t=0; t<traceMatrix.size(); t++)
        {

            glColor3f(0.0, 0.0, 1.0);
            glBegin(GL_LINE_STRIP);

            bool skipLow = false, skipHigh = false;
            for (c=0; c<traceMatrix[0].size(); c++)
            {
                s = traceMatrix[t][c];
                if (s==low)
                {
                    y = (traceboxHeight + margin - unitHeight - 2.5*unitHeight*t + canvasPosition*5);
                    x = margin + labelWidth + unitWidth*c;
                }

                if (s==high)
                {
                    y = (traceboxHeight + margin - 2.5*unitHeight*t + canvasPosition*5);
                    x = margin + labelWidth + unitWidth*c;
                }
                /*if (y > (height-canvasPosition))
                {
                    y = height - canvasPosition;
                    skipHigh = true;
                    glEnd();
                }
                else
                {
                    skipHigh = false;
                    glBegin(GL_LINE_STRIP);
                }*/
                glVertex2f(x, y);
                glVertex2f(x+unitWidth, y);
            }
            glEnd();
        }

        // Write out labels for the traces
        for (int j=0; j<monitorNameVector.size(); j++)
        {
            y = (traceboxHeight-1 - 2.5*unitHeight*j + canvasPosition*5);
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

  /*// If there are no montiors then draw an artificial trace.
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
        }

        for (int j=0; j<monitorNameVector.size(); j++)
        {
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
    */


    // Example of how to use GLUT to draw text on the canvas
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(margin, 0.1*height);
    //for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);



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

void MyGLCanvas::setCanvasScrollBar()
{
    int scrollHeight = unitHeight/5;
    int numPositions = scrollHeight * 2.5 * (monitorNameVector.size()-1);
    //cout << "scrollHeight = " << scrollHeight << ", numPositions = " << numPositions << endl;
    SetScrollbar(wxVERTICAL,0,10,10+numPositions);
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
    canvasPosition = event.GetPosition();
    //cout << "Canvas position is " << canvasPosition << endl;
    Render(wxT("Scrolling"),-1);
}


void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Callback function for when the canvas is resized
{
  wxGLCanvas::OnSize(event); // required on some platforms
  init = false;
  setCanvasScrollBar();
  canvasPosition = 0;
  Refresh(); // required by some buggy nvidia graphics drivers,
  Update();  // harmless on other platforms!
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Callback function for mouse events inside the GL canvas
{
  wxString text;
  int w, h;;

  GetClientSize(&w, &h);
  //if (event.ButtonDown()) text.Printf(wxT("Mouse button %d pressed at %d %d"), event.GetButton(), event.m_x, h-event.m_y);
  //if (event.ButtonUp()) text.Printf(wxT("Mouse button %d released at %d %d"), event.GetButton(), event.m_x, h-event.m_y);
  //if (event.Dragging()) text.Printf(wxT("Mouse dragged to %d %d"), event.m_x, h-event.m_y);
  //if (event.Leaving()) text.Printf(wxT("Mouse left window at %d %d"), event.m_x, h-event.m_y);

  //if (event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text,-1);
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

// MyFrame *******************************************************************************************************************************
//         *******************************************************************************************************************************
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_MENU(wxID_NEW,MyFrame::OnNew)
  

  EVT_BUTTON(RUN_BUTTON,  MyFrame::OnRunButton)
  EVT_BUTTON(CONT_BUTTON, MyFrame::OnContButton)

  EVT_BUTTON(ZAP_TRACE_BUTTON, MyFrame::OnButtonZap)
  EVT_BUTTON(ADD_TRACE_BUTTON, MyFrame::OnButtonAdd)

  EVT_BUTTON(SWITCH_BUTTON_0, MyFrame::OnButtonSwitch0)
  EVT_BUTTON(SWITCH_BUTTON_1, MyFrame::OnButtonSwitch1)

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
    SetIcon(wxIcon(wx_icon));
    nmz = names_mod;
    dmz = devices_mod;
    mmz = monitor_mod;
    netz = network_mod;

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

    fileMenu->Append(wxID_NEW ,wxT("&New"));
    fileMenu->Append(wxID_OPEN ,wxT("&Open"));
    fileMenu->Append(wxID_ABOUT, wxT("&About"));
    fileMenu->Append(wxID_EXIT, wxT("&Quit"));


    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(viewMenu, wxT("&View"));
    menuBar->Append(searchMenu, wxT("&Search"));
    menuBar->Append(toolsMenu, wxT("&Tools"));


    SetMenuBar(menuBar);;
    // Define the Trace Canvas
    canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, network_mod);
    cyclescompleted = 0;
    runnetwork(10);
    canvas->populateMonitorNameVector();
    populateSwitchNameVector();
    canvas->setCyclesCompleted(cyclescompleted);
    canvas->populateTraceMatrix();

    canvas->setCanvasScrollBar();
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
    wxString zapTraceList[numDevices];
    wxString addTraceList[numDevices + canvas->numDtypes];
    wxString switchList[numSwitches];


    // Make the zap combobox strings
    for (int i=0; i<numMonitors; i++)
    {
        zapTraceList[i] = canvas->monitorNameVector[i];
    }
    
    // Make the add combobox strings
    for (int i=0; i<numDevices; i++)
    {        
        wxString deviceNamestring = canvas->deviceNameVector[i];

		name_t did = getIdFromWxString(deviceNamestring);
        devicekind dkind = netz->netzdevkind(did);
        if (dkind == dtype)
        {   
            addTraceList[c] = deviceNamestring.Append(wxT(".Q"));
            c++;
            addTraceList[c] = deviceNamestring.Append(wxT("BAR"));
            c++;
        }
        else
        {
            addTraceList[c] = deviceNamestring;
            c++;
        }
    }

    // Make the switch combo-box strings
    for (int i=0; i<numSwitches; i++)
    {
        switchList[i] = switchNameVector[i];
    }




  // DEFINITIONS OF BUTTONS, CONTROLS AND SIZERS
  // *********************************************************************************************************************************

  //Define Zap Controls
    zapTraceComboBox = new wxComboBox(this,ZAP_TRACE_COMBO_BOX, wxT("Choose trace to zap"),wxDefaultPosition, wxDefaultSize,canvas->monitorNameVector.size(),zapTraceList);
    zapTraceButton = new wxButton(this, ZAP_TRACE_BUTTON, wxT("ZAP"));

  // Define Add Controls
    addTraceComboBox = new wxComboBox(this,ADD_TRACE_COMBO_BOX, wxT("Choose trace to add"),wxDefaultPosition, wxDefaultSize,canvas->deviceNameVector.size()+canvas->numDtypes,addTraceList);
    addTraceButton = new wxButton(this, ADD_TRACE_BUTTON, wxT("ADD"));

  // Define Switch Controls
    switchComboBox = new wxComboBox(this,SWITCH_COMBO_BOX, wxT("Choose Switch "), wxDefaultPosition, wxDefaultSize, numSwitches, switchList);
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

void MyFrame::OnAbout(wxCommandEvent &event)
  // Callback for the about menu item
{
    wxString text = wxT("Example");
}

void MyFrame::OnOpen(wxCommandEvent &event)
{
    wxFileDialog *openDialog = new wxFileDialog(this,wxT("Open file"), wxT(""), wxT(""), wxT("*.def"),wxFD_OPEN, wxDefaultPosition);
    openDialog->ShowModal();
    wxString fid = openDialog->GetPath();
    system("./logsim fid");
    return;
}

void MyFrame::OnNew(wxCommandEvent &event)
{
    system("gedit untitled.def");
}


// Callback for the run button
void MyFrame::OnRunButton(wxCommandEvent &event)
{
  // Call the run function to run the network for the number of cycles currently in the
    RunFunction();
}

// Callback for the continue button
//****************************************************************************************************************************************
void MyFrame::OnContButton(wxCommandEvent &event)
{
    if (cyclescompleted > 2700)
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
    canvas->Render(wxT("Run button pressed"),cyclescompleted);
    wxString commandLineText;
    commandLineText.Printf(wxT("# Network continued for %d cycles\n"),newcycles);
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
}
//****************************************************************************************************************************************

void MyFrame::OnButtonZap(wxCommandEvent &event)
  // Callback for Zap PushButton
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
        {
            errorBox(wxT("Sorry we couldn't find the monitor you tried to zap"));
            return;
        }
        return;
    }

  // Now that we've confirmed that the selection is valid, clear the traceMatrix
//    canvas->traceMatrix.clear();

  // Find namestring of device
    string devNamestring = string(selectionStr.mb_str());

    wxString text;
    text.Printf(wxT("# %s removed\n"),selectionStr.c_str());

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
    canvas->Render(text,-1);
    commandLine->WriteText(text);
    commandLine->SetInsertionPoint(0);

  // Reset the text in the ComboBox
    zapTraceComboBox->SetValue(wxT("Choose trace to zap"));

  // Resize the scroll bar
    canvas->setCanvasScrollBar();
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
    
    cout << "deviceName =  " << string(deviceName.mb_str()) << endl;
    cout << "outputName =  " << string(outputName.mb_str()) << endl;
    cout << "monitorName =  " << string(monitorName.mb_str()) << endl;
    
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
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);

  // Run the network for a few values
    RunFunction();

  // Reset the text in the ComboBox
    addTraceComboBox->SetValue(wxT("Choose trace to add"));

  // Resize the scroll bar
    canvas->setCanvasScrollBar();
}

void MyFrame::OnButtonSwitch0(wxCommandEvent &event)
  // Callback for second pushbutton
{
    if (switchComboBox->GetStringSelection().IsEmpty())
    {
        errorBox(wxT("You need to select a device to monitor"));
        addTraceComboBox->SetValue(wxT("Choose trace to add!"));
        return;
    }

    name_t sid = getIdFromWxString(switchComboBox->GetStringSelection());
    // Following for debugging
    //namestring_t devStr = string(switchComboBox->GetStringSelection().mb_str());
    //name_t sid = nmz->cvtname(devStr);
    asignal s = low;
    bool ok;

    dmz->setswitch(sid, s, ok);
    if (!ok)
        cout << "Error setting switch to 0" << endl;

    wxString swStr = switchComboBox->GetStringSelection();
    wxString commandLineText;
    commandLineText.Printf(wxT("# %s changed to 0\n"),swStr.c_str());
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
}

void MyFrame::OnButtonSwitch1(wxCommandEvent &event)
  // Callback for second pushbutton
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
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
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
    commandLine->SetInsertionPoint(0);
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
  {
    cyclescompleted = cyclescompleted + ncycles;
  }
  else
  {
    cyclescompleted = 0;
  }
}

void MyFrame::aboutfunction(wxString traceStr, wxString switchStr)
{
  wxString message;
  message.Printf(wxT("Trace Selected: %s \nSwitch Selected: %s"),traceStr.c_str(),switchStr.c_str());
  wxMessageDialog about(this,message,wxT("About"), wxICON_INFORMATION | wxOK);
  about.ShowModal();
  return;
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
    commandLine->WriteText(commandLineText);
    commandLine->SetInsertionPoint(0);
 
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
