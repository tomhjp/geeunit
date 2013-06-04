#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "scanner.h"
#include "parser.h"
#include <string>
#include <cstring>
#include <vector>

// widget identifiers
enum {
  RUN_SPINCTRL = wxID_HIGHEST + 1,
  CONT_SPINCTRL,
  COMMAND_LINE,
  RUN_BUTTON,
  CONT_BUTTON,
  ZAP_TRACE_BUTTON,
  ADD_TRACE_BUTTON,
  SWITCH_BUTTON_0,
  SWITCH_BUTTON_1,
  ZAP_TRACE_COMBO_BOX,
  ADD_TRACE_COMBO_BOX,
  SWITCH_COMBO_BOX,
  OPEN_DIALOG,
  ROLLING_TIMER = wxID_HIGHEST,
  START_TIMER_BUTTON,
  STOP_TIMER_BUTTON,
  VERTICAL_SCROLL_BAR,
}; 

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
    MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
      names *names_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, network *network_mod = NULL, scanner_t *smz = NULL, parser *pmz = NULL,
      long style = wxDEFAULT_FRAME_STYLE); // constructor
    private:
        MyGLCanvas *canvas;                     // GL drawing area widget to draw traces

        names *nmz;                             // pointer to names class
        devices *dmz;                           // pointer to devices class
        monitor *mmz;                           // pointer to monitor class
        network *netz;                          // pointer to network class
        scanner_t *smz;                         // pointer to scanner class
        parser *pmz;                            // pointer to parser class

                                                    
  // Pointers to all the buttons Objects
        wxButton     *zapTraceButton;
        wxButton     *addTraceButton;                      
        wxButton     *switchButton0;
        wxButton     *switchButton1;
        wxButton     *runButton;
        wxButton     *contButton;
        wxButton     *startTimerButton;
        wxButton     *stopTimerButton;
        
  // Pointers to comboBoxes Objects
        wxComboBox   *zapTraceComboBox;           
        wxComboBox   *addTraceComboBox;
        wxComboBox   *switchComboBox;
        
  // Pointers to SpinControl Objects
        wxSpinCtrl   *runSpin;
        wxSpinCtrl   *contSpin;  
  
  // Pointers to StaticText objects
        wxStaticText *runStaticText;
        wxStaticText *contStaticText;
        
  // Pointer to Misc Controls
        wxTextCtrl   *commandLine;
        wxFileDialog *openDialog;
        wxTimer      *rollingTimer;

  // Defining some variables used by the Canvas Object
        vector<wxString> switchNameVector;
        int  FRAMEcyclesCompleted;                  // How many cycles were completed in the most recent run.
        int  FRAMEtotalCycles;                      // Total number of cycles completed.
                                                    
  // Callbacks for fileMenu events                                   
        void OnExit(wxCommandEvent& event);         
        void OnOpen(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);

  // callback for all GUI Controls
        void OnRunButton(wxCommandEvent& event);   
        void OnContButton(wxCommandEvent& event);
        void OnButtonZap(wxCommandEvent& event);
        void OnButtonAdd(wxCommandEvent& event);
        void OnButtonSwitch0(wxCommandEvent& event);
        void OnButtonSwitch1(wxCommandEvent& event);
        void OnText(wxCommandEvent& event);     
        void OnButtonStartTimer(wxCommandEvent& event);
        void OnButtonStopTimer(wxCommandEvent& event);     
        void OnTimer(wxTimerEvent& event);
        
  // Methods used by the MyFrame Class
        void runnetwork(int ncycles); 
        void RunFunction();
        void ContinueFunction();
        void errorBox(wxString errorBox);
        bool isdtype(name_t did);
        void checkMonitorName(wxString monitorName, wxString& deviceName, wxString& outputName, bool& isDtype);
        void resetCanvas();
        void OpenFile();
        void WriteToCommandLine(wxString commandLineText);
        void populateSwitchNameVector();
        name_t getIdFromWxString(wxString inStr);

        DECLARE_EVENT_TABLE()
};

class MyGLCanvas: public wxGLCanvas
{
    public:
    MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,network* network_mod = NULL,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("MyGLCanvas")); // constructor

  // Define Public methods of the Canvas Class
    void Render(int totalCycles);
    void populateTraceMatrix();
    void appendToTraceMatrix();
    void populateMonitorNameVector();
    void setCANVAStotalCycles(int c);
    void setCANVAScyclesCompleted(int c);
    void setCanvasVerticalScrollBar();
    void setCanvasHorizontalScrollBar(int position);
    void setHorizontalPosition(int position);
    void setVerticalPosition(int position);
    void setNames(names* names_mod);
    void setMonitor(monitor* monitor_mod);
    void setNetwork(network* network_mod);
    void setClearToRunFlag(bool flag);

  // Define some variables used by the Canvas Class
    typedef vector<vector<asignal> > IntMatrix;
    IntMatrix traceMatrix;
    
    vector<wxString> deviceNameVector;
    vector<wxString> monitorNameVector;
    
    int numDtypes;
    bool clearToRun;

    private:
  // Variable initialised on start-up and never altered. 
        bool init;
        monitor *mmz;                      
        names *nmz;
        network *netz;
        
  // Variables defined for drawing on the Canvas                                          
        int CANVAStotalCycles;                          
        int CANVAScyclesCompleted;
        int unitHeight;
        int unitWidth;
        
  // Variables to do with the Scroll Bar              
        wxScrollBar *verticalScrollBar;
        unsigned int vertCanvasPosition;
        unsigned int horizCanvasPosition;

  //  Event Callbacks for the Canvas
        void InitGL();                    
        void OnSize(wxSizeEvent& event);   
        void OnPaint(wxPaintEvent& event); 
        void OnScroll(wxScrollWinEvent& event);
        
        DECLARE_EVENT_TABLE()
};
#endif /* gui_h */
