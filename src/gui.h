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
#include <string>
#include <cstring>
#include <vector>

enum { 
  RUN_SPINCTRL = wxID_HIGHEST + 1,
  CONT_SPINCTRL,
  COMMAND_LINE,
  RUN_BUTTON,
  CONT_BUTTON,
  ZAP_TRACE_BUTTON,
  ADD_TRACE_BUTTON,
  SWITCH_BUTTON_1,
  SWITCH_BUTTON_2,
  ZAP_TRACE_COMBO_BOX,
  ADD_TRACE_COMBO_BOX,
  SWITCH_COMBO_BOX
}; // widget identifiers

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, 
	  names *names_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, 
	  long style = wxDEFAULT_FRAME_STYLE); // constructor
 private:
    MyGLCanvas *canvas;                     // GL drawing area widget to draw traces

    names *nmz;                             // pointer to names class
    devices *dmz;                           // pointer to devices class
    monitor *mmz;                           // pointer to monitor class
    
    wxSpinCtrl   *runSpin;
    wxSpinCtrl   *contSpin;                       // control widget to select the number of cycles
    wxComboBox   *zapTraceComboBox;           // pointer to combobox object
    wxButton     *zapTraceButton;
    wxComboBox   *addTraceComboBox;
    wxButton     *addTraceButton;
    wxComboBox   *switchComboBox;             // pointer to combobox object
    wxButton     *switchButton1;
    wxButton     *switchButton2;
    wxButton     *runButton;
    wxButton     *contButton;
    wxStaticText *runStaticText;
    wxStaticText *contStaticText;
    wxTextCtrl   *commandLine;
    
    int cyclescompleted;                    // how many simulation cycles have been completed
    void runnetwork(int ncycles);           // function to run the logic network
    void OnExit(wxCommandEvent& event);     // callback for exit menu item
    void OnAbout(wxCommandEvent& event);    // callback for about menu item
    void OnRunButton(wxCommandEvent& event);   // callback for push button
    void OnContButton(wxCommandEvent& event);
    void OnButtonZap(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonSwitch1(wxCommandEvent& event);
    void OnButtonSwitch2(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnSpin(wxSpinEvent& event);        // callback for spin control
    void OnText(wxCommandEvent& event);     // callback for text entry field
    void aboutfunction(wxString traceStr, wxString switchStr);       // callback for creating about box. 
    DECLARE_EVENT_TABLE()
};
    
class MyGLCanvas: public wxGLCanvas
{
    public:
    MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
   	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = wxT("MyGLCanvas")); // constructor

    void Render(wxString example_text = wxT(""), int cycles = -1); // function to draw canvas contents
    
    vector<int> traceVector;
        
    private:
        bool init;                         // has the GL context been initialised?
        int cyclesdisplayed;               // how many simulation cycles have been displayed
        monitor *mmz;                      // pointer to monitor class, used to extract signal traces
        names *nmz;                        // pointer to names class, used to extract signal names
        void InitGL();                     // function to initialise GL context
        void OnSize(wxSizeEvent& event);   // callback for when canvas is resized
        void OnPaint(wxPaintEvent& event); // callback for when canvas is exposed
        void OnScroll(wxScrollWinEvent& event);
        void OnMouse(wxMouseEvent& event); // callback for mouse events inside 
        DECLARE_EVENT_TABLE()
};
    
#endif /* gui_h */
