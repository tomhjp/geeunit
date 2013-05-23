#include <iostream>
#include "parser.h"
#include "error.h"

using namespace std;

/* The parser for the circuit definition files */


bool parser::readin (void)
{  
    /* Used to move through the file, and build up the context vector, curline */ 
    int num; 
    namestring_t namestring;
    symbol_t symbol;
    
    /* read in symbols from the scanner and build up the curline vector */ 
    smz->nextSymbol(symbol, namestring, num); 
    if(symbol != startfsym) addError(noStrtFile); 
    smz->nextSymbol(symbol, namestring, num); 
    if(symbol == endfsym) return true;
    else if(symbol != devsym) addError(expDevSym);    
    // 1. build up context vector    
    // 2. call syntax checker
    // 3. if(syntax = good) call semantics checker
    // 4. if(semantics = good) make dev/con/mon 
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
}

void parser::setSection(section_t sec)
{
    section = sec; 
    return;
}

int parser::checkSyntax(section_t sec)
{
    /* calls the checkSyntax method from the relevant syntax class */
    /* use class pointers (?) as done above */   
}

