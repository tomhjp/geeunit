#include <iostream>
#include "scanner.h"
#include "parser.h"
#include "error.h"

using namespace std;

/* The parser for the circuit definition files */

void parser::preStartFCheck(symbol_t symbol)
{
    context.pushback(symbol); 
    if(symbol != startfsym)
    { 
       errorvector.pushback(new noStrtFile(symbol.line, symbol.col);
       skipToBreak(); 
    }
    return;
}

void parser::setSection(section_t sec)
{
    section = sec; 
    return;
}


/*****************************************************************************/
/****************** Public methods *******************************************/

/* read in symbols from the scanner, and calls the relevant parsing functions */ 

void parser::readin (symbol_t symbol)
{   
    if(section = prestartf) preStartFCheck(symbol); 
    else if(section = devsect) devSectCheck(symbol); 
    else if(section = monsect) monSectCheck(symbol);
    else if(section = consect) conSectCheck(symbol);
    else if(section = postendf) postEndFCheck(symbol);
    else cout << "Erm, something's gone wrong" << endl; //PANIC.        
}

vector<Error*> parser::getErrorVector(void)
{
    return errorvector; 
}

parser::parser (network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner_t* scanner_mod)
{
    netz = network_mod;  /* make internal copies of these class pointers */
    dmz = devices_mod;   /* so we can call functions from these classes  */
    mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
    smz = scanner_mod;   /* class you say:                               */
                         /* netz->makeconnection (i1, i2, o1, o2, ok);   */

    /* any other initialisation you want to do? */
    section = prestartf; 
}

