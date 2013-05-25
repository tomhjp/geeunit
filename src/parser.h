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
    
    int needskeyflag;   // used to check for the next keyword, as defined by the section
    int skipflag;       // used to tell parser to ignore rest of line up to keyword or ';'
    int nodevsymflag;   // prevent repeated "no KEYWORD keyword" errors
    int noconsymflag;
    int nomonsymflag; 
    int noendfsymflag;
    
    /* Vector of pointers to errors. returned to main after the whole file has been parsed */
    vector<Error*> errorvector;
    
    /************* Private Functions ****************************************/ 
    void preStartFCheck(symbol_t symbol);	//check functions for each of the sections of the file 
    void devSectCheck(symbol_t symbol);
    void conSectCheck(symbol_t symbol);
    void monSectCheck(symbol_t symbol);
    void postEndFCheck(symbol_t symbol); 
    void skipToBreak(void);					//skips to the next END or ;
    void nextKeyWordCheck(symbol_t symbol); //checks that the next symbol after END, etc, is as expected
    bool checkDevLine(void);
    
    
  public:
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */
    void readin (symbol_t symbol);
    
    /* Populated with pointers to Error objects BY THE PARSER */
    /* called after hte whole file has been parsed             */  
    vector<Error*> getErrorVector(void); 
    
    /* the constructor takes pointers to various other classes as parameters */
    parser (network* network_mod, devices* devices_mod,
	      monitor* monitor_mod, scanner_t* scanner_mod, names* names_mod);

};

#endif /* parser_h */

