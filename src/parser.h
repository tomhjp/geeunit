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


typedef enum {devsect, consect, monsect} section_t; 


class parser {
    network* netz; // instantiations of various classes for parser to use.
    devices* dmz; 
    monitor* mmz;
    scanner_t* smz;
    
    /* context vector - contains the symbols that make up the current line */ 
    vector<symbol_t> curline; 

    /* errorvector - contains pointers to the Error objects created by parsing */ 
    vector<Error*> errorvector; 

    /* Used to define which syntax/semantic rules to apply */ 
    section_t section;
    
  public:
    /* Reads the definition of the logic system and builds the             */
    /* corresponding internal representation via calls to the 'Network'    */
    /* module and the 'Devices' module.                                    */
    bool readin ();
    
    /* gets the context vector for the current line for syntax checking classes */
    /* (NB. syntax can be checked without examining the exact input, just check */
    /* that the symbol type input is correct)                                   */
    vector<symbol_t> getContext(void);  
    
    /* sets the section currently being parsed */ 
    void setSection(section_t sec); 
    
    /* calls the relevant syntax checking class */ 
    /* return value is an error code. 0 = syntax check passed  */ 
    int checkSyntax(section_t sec);     
    
    /* creates a new Error object of the correct subclass type and skips forwards */ 
    /* in the definition file to the next valid point                           */ 
    void addError(errorcode_t errorcode);
    
    /* the constructor takes pointers to various other classes as parameters */
    parser (network* network_mod, devices* devices_mod,
	      monitor* monitor_mod, scanner_t* scanner_mod);

};

#endif /* parser_h */

