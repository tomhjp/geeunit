#ifndef parser_h
#define parser_h

#include "names.h"
#include "scanner.h"
#include "network.h"
#include "devices.h"
#include "monitor.h"
#include "error.h"
#include <vector> 

using namespace std;


typedef enum {prestartf, devsect, consect, monsect, postendf} section_t; 


class parser {
    network* netz; // instantiations of various classes for parser to use.
    devices* dmz; 
    monitor* mmz;
    scanner_t* smz;
    
    /* context vector - contains the symbols that make up the current line */ 
    vector<symbol_t> context;  
    /* Used to define which syntax/semantic rules to apply */ 
    section_t section;
    /* Vector of errors. returned to main after the whole file has been parsed */
    vector<Error*> errorvector;
    
    /************* Private Functions ****************************************/ 
    void preStartFCheck(symbol_t symbol);
    void devSectCheck(symbol_t symbol);
    void conSectCheck(symbol_t symbol);
    void monSectCheck(symbol_t symbol);
    void postEndFCheck(symbol_t symbol);
    void checkPreStartFSyntax(void); 
    void skipToBreak(void);
    void setSection(section_t sec); 
    
    
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
	      monitor* monitor_mod, scanner_t* scanner_mod);

};

#endif /* parser_h */

