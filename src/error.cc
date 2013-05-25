#include "error.h"
#include "names.h"
#include <iostream>
#include <string>

using namespace std; 


/**************************************************************************/
/* Methods for the superclass Error  */ 

/* Prints out the error message and line where the error occurred */ 
void Error::printErrMsg(void) 
{
   cout << "Error: " << line << ", " << col << ": " <<errorMessage <<endl;
} 

Error::Error()
{
	return;
}

/******************************************************************************/
/************ Methods for subclasses (ie. specific errors)   ******************/ 
/******************************************************************************/


/*********     'Simple' error types first       *******************************/ 

noStrtFile::noStrtFile(int l, int c) 
{
    errorMessage = "Expected 'STARTFILE' keyword";
    line = l;
    col = c;
}

expDeviSym::expDeviSym(int l, int c)
{
    errorMessage = "Expected 'DEVICES' keyword"; 
    line = l;
    col = c;
}

expMonSym::expMonSym(int l, int c)
{
	errorMessage = "Expected 'MONITORS' keyword";
	line = l;
	col = c;
}

expConSym::expConSym(int l, int c)
{
	errorMessage = "Expected 'CONNECTIONS' keyword"; 
	line = l;
	col = c; 
}

expEndFSym::expEndFSym(int l, int c)
{
	errorMessage = "Expected 'ENDFILE' keyword"; 
	line = l;
	col = c;
}

noSemiCol::noSemiCol(int l, int c)
{
	errorMessage = "No semicolon before 'END' keyword";
	line=l;
	col = c;
}

expDevName::expDevName(int l, int c)
{
	errorMessage = "Expected a device name";
	line = l;
	col = c;
}

expEqualSym::expEqualSym(int l, int c)
{
	errorMessage = "Expected an '=' symbol";
	line = l;
	col = c;
}

expOPSym::expOPSym(int l, int c)
{
	errorMessage = "Expected a '(' symbol - paramenters must be declared in parentheses";
	line = l; 
	col = c;
}

expNumSym::expNumSym(int l, int c)
{
	errorMessage = "Expected a number - parameters must be numbers";
	line = l;
	col = c;
}	
	
paramRangeErrSwitch::paramRangeErrSwitch(int l, int c)
{
	errorMessage = "Parameter out of valid range - SWITCH device parameters must be either 0 or 1";
	line = l;
	col = c;
}

paramRangeErrGate::paramRangeErrGate(int l, int c)
{
	errorMessage = "Parameter out of valid range - the valid range is 1-16 (inclusive)";
	line = l; 
	col = c;
}

expCPSym::expCPSym(int l, int c)
{
	errorMessage = "Expected a ')' symbol - parameters must be declared in parentheses. You may have exceeded the number of parameters for this device type";
	line = l;
	col = c;
}

expSemiColSym::expSemiColSym(int l, int c)
{
	errorMessage = "Expected a ';' symbol.  The parser thinks this should be the end of the line"; 
	line = l; 
	col = c;
}

		


	
	
/*****************************************************************************************/
/**************** Methods for classes requiring prior information ************************/
/*****************************************************************************************/
/*****************  eg. if a device name is already defined, needs line, col *************/ 


nameAlreadyDefd::nameAlreadyDefd(int l, int c, namestring_t dev, names* names_mod)
{
	errorMessage = ": This device name has already been defined, at ";
	line = l;
	col = c; 
	devname = dev;
	nmz = names_mod;
}

void nameAlreadyDefd::printErrMsg(void)
{
	getInitDef();
	cout << "Error: " << line << ", " << col <<": " << devname << errorMessage << initdefline << ", " << initdefcol <<endl;
}

void nameAlreadyDefd::getInitDef(void)
{
	initdefline = nmz->getLine(devname);
	initdefcol = nmz->getCol(devname);
}



