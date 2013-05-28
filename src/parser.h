#ifndef parser_h
#define parser_h

#include <vector> 
#include "names.h"
#include "typedefs.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"
#include "device.h"
#include "warning.h"

using namespace std;


typedef enum {prestartfsect, devsect, consect, monsect, postendfsect} section_t; 


class parser {
    network* netz; // instantiations of various classes for parser to use.
    devices* dmz; 
    monitor* mmz;
    scanner_t* smz;
    names* nmz;
    
    /* context vector - contains the symbols that make up the current line */ 
    vector<symbol_t> context;  
    /* Used to define which syntax/semantic rules to apply */ 
    section_t section;
    
    /* Flags.  Set = 1, notSet = 0  */ 
    int needskeyflag;   // used to check for the next keyword, as defined by the section
    int skipflag;       // used to tell parser to ignore rest of line up to keyword or ';'
    int nodevsymflag;   // prevent repeated "no KEYWORD keyword" errors
    int noconsymflag;
    int nomonsymflag; 
    int noendfsymflag;
    int filenotcompleteflag;  	// used to check the file is 'complete', ie. eofile has been detected before
				// all sections and keywords have been detected 
    
    
    name_t      clkpin, datapin, setpin;
    name_t      clrpin, qpin, qbarpin;     /* Input and Output Pin names for dtype devices */
    
    /* Vector of pointers to errors. returned to main after the whole file has been parsed */
    vector<Error*> errorvector;
    vector<Warning*> warningvector;
    
    /************* Private Functions ****************************************/ 	
    void mainLineBuild(symbol_t symbol);    // performs line building and calls line checking functions for DEV, CON, MON sections
    void skipToBreak(symbol_t symbol);		    //skips to the next END or ;
    void nextKeyWordCheck(symbol_t symbol); //checks that the next symbol after END, etc, is as expected
    void preStartFCheck(symbol_t symbol);   // check functions for each of the sections of the file 
    bool checkDevLine(void);
    bool checkConLine(void);
    bool checkMonLine(void);
    void postEndFCheck(symbol_t symbol);
    void endSection(void);
    bool makeDevLine(void);                 // carry out the operations defined by the line in the definition file
    bool makeConLine(void);                 // once the line has been parsed and if total number of errors is zero 
    bool makeMonLine(void);
    void emptyContextVector(void);
    
    bool isStrSym(symbol_t symbol);
    bool isConnPuncSym(symbol_t symbol);
    bool isDotSym(symbol_t symbol);
    bool isSemiColSym(symbol_t symbol);
    bool isDtypeInput(symbol_t symbol);
    bool isDtypeOutput(symbol_t symbol);
    bool devNameDefined(symbol_t symbol);
    bool gateInputDefined(symbol_t symbol);
    bool gateInputUnconnected(symbol_t symbol);
    bool dtypeInputUnconnected(symbol_t symbol);
    
  public:
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */
    void readin (symbol_t symbol);
    
    /* Populated with pointers to Error objects BY THE PARSER */
    /* called after hte whole file has been parsed             */  
    vector<Error*> getErrorVector(void); 
    
    /* Populated with pointers to Warning objects by the parser */ 
    /* called after the whole file has been parsed 				*/ 
    vector<Warning*> getWarningVector(void);
    
    /* the constructor takes pointers to various other classes as parameters */
    parser (network* network_mod, devices* devices_mod,
	      monitor* monitor_mod, scanner_t* scanner_mod, names* names_mod);

};

#endif /* parser_h */

