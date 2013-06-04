#include "error.h"
#include "names.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std; 


/**************************************************************************/
/* Methods for the superclass Error  */ 

/* Prints out the error message and line where the error occurred */ 
void Error::printErrMsg(void) 
{
   cout << "Error: " << line << ", " << col << ": " <<errorMessage <<endl;
} 

/* used by the scanner to get the error details.  */ 
void Error::getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition)
{
	l = line; 
	c = col; 
    hasPosition = hasPos; 
	errmsg = "Error: " + errorMessage;
    return;
}

Error::Error()
{
    hasPos = true;
    return;
}

/******************************************************************************/
/************ Methods for subclasses (ie. specific errors)   ******************/ 
/******************************************************************************/


/*********     'Simple' error types first       *******************************/ 

noStrtFile::noStrtFile(int l, int c) 
{
    errorMessage = "Expected 'STARTFILE' keyword (keywords must be upper case)";
    line = l;
    col = c;
}

expDeviSym::expDeviSym(int l, int c)
{
    errorMessage = "Expected 'DEVICES' keyword (keywords must be upper case, and sections must be correctly ordered)"; 
    line = l;
    col = c;
}

expMonSym::expMonSym(int l, int c)
{
	errorMessage = "Expected 'MONITORS' keyword (keywords must be upper case, and sections must be correctly ordered)";
	line = l;
	col = c;
}

expConSym::expConSym(int l, int c)
{
	errorMessage = "Expected 'CONNECTIONS' keyword (keywords must be upper case, and sections must be correctly ordered)"; 
	line = l;
	col = c; 
}

expEndFSym::expEndFSym(int l, int c)
{
	errorMessage = "Expected 'ENDFILE' keyword (keywords must be upper case)"; 
	line = l;
	col = c;
}

noSemiCol::noSemiCol(int l, int c)
{
	errorMessage = "No semicolon before 'END' keyword (keywords must be upper case)";
	line=l;
	col = c;
}

expDevName::expDevName(int l, int c)
{
	errorMessage = "Expected a device name. Device names must begin with an alphabetic character, and may not be keywords.";
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

expDevTypeSym::expDevTypeSym(int l, int c)
{
	errorMessage = "Expected a device type keyword (eg. DTYPE, CLOCK, etc.)"; 
	line = l;
	col = c; 
}
	
paramRangeErrSwitch::paramRangeErrSwitch(int l, int c)
{
	errorMessage = "Parameter out of valid range - SWITCH device parameters must be either 0 or 1";
	line = l;
	col = c;
}

param1RangeErrSigGen::param1RangeErrSigGen(int l, int c)
{
	errorMessage = "Parameter 1 out of valid range - SIGGEN period device parameters must in the range 1-65535";
	line = l;
	col = c;
}

paramNRangeErrSigGen::paramNRangeErrSigGen(int l, int c)
{
	errorMessage = "Parameter out of valid range - SIGGEN pattern device parameters must be 1 or 0";
	line = l;
	col = c;
}

paramRangeErrGate::paramRangeErrGate(int l, int c)
{
	errorMessage = "Parameter out of valid range - the valid range is 1-16";
	line = l; 
	col = c;
}

paramRangeErrClk::paramRangeErrClk(int l, int c)
{
	errorMessage = "Parameter out of valid range - the valid range is 1-65535";
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

expCommaSym::expCommaSym(int l, int c)
{
    errorMessage = "Expected a ',' symbol between parameters, or a ')' symbol if all parameters have been input";
    line = l; 
    col = c; 
}

unExpEndSym::unExpEndSym(int l, int c)
{
    errorMessage = "Unexpected END symbol.  Did you forget a semicolon?";
    line = l; 
    col = c;
}

devNameUndef::devNameUndef(int l, int c)
{
    errorMessage = "Undefined device name";
    line = l;
    col = c;
}

expConnPuncSym::expConnPuncSym(int l, int c)
{
    errorMessage = "Expected '->' symbol";
    line = l; 
    col = c;
}

expDotSym::expDotSym(int l, int c)
{
    errorMessage = "Expected '.' symbol." ;
    line = l;
    col = c;
}	

expDtypeInput::expDtypeInput(int l, int c)
{
    errorMessage = "Expected a dytpe input. Valid inputs are 'DATA', 'CLK', 'CLEAR', and 'SET'";
    line = l; 
    col = c;
}

expDtypeOutput::expDtypeOutput(int l, int c)
{
    errorMessage = "Expected a dtypeoutput. Valid outputs are 'Q' and 'QBAR'";
    line = l; 
    col = c;
}

expInputSym::expInputSym(int l, int c)
{
    errorMessage = "Expected a gate input.";
    line = l; 
    col = c;
}

invInputSym::invInputSym(int l, int c)
{
    errorMessage = "The input string entered is invalid";
    line = l; 
    col = c;
}

inputUnDefd::inputUnDefd(int l, int c)
{
    errorMessage = "The input referenced does not exist";
    line = l; 
    col = c;
}

unconnectInp::unconnectInp(string list)
{
    errorMessage = "Unconnected input(s) detected (listed below)";
    hasPos = false;
    errorMessage = errorMessage + "\n" + list; 
}	

badDevType::badDevType(int l, int c)
{
    errorMessage = "Invalid devicetype entered"; 
    line = l;
    col = c;
}

lineBuildFailed::lineBuildFailed(int l, int c)
{
    errorMessage = "Parser error occurred.  This line parsed correctly, but failed to build";
    line = l;
    col = c;
}

foundSymAfterEndf::foundSymAfterEndf(int l, int c)
{
	errorMessage = "Symbol found after ENDFILE";
	line = l; 
	col = c;
}


overMaxMonLimit::overMaxMonLimit(int l, int c)
{
    errorMessage = "Maximum number of monitors (50) has been exceeded - monitor was not made";
    line = l; 
    col = c;
}

fileNotComplete::fileNotComplete()
{
    errorMessage = "Input file is not complete.  Have you included all sections and required keywords?";
    hasPos = false;
}

fatalErr::fatalErr(int l, int c)
{
    errorMessage = "A fatal parser error has occurred - sorry! Check your input file and try again"; 
    line = l; 
    col = c; 
}

	
/*****************************************************************************************/
/**************** Methods for classes requiring extra information ************************/
/*****************************************************************************************/
/*****************  eg. if a device name is already defined, needs line, col *************/ 

inputPrevConnected::inputPrevConnected(int l, int c, name_t id, devlink dev, network* network_mod)
{
	line = l; 
    col = c;
    netz = network_mod;
    devicelink = dev; 
    ipid = id;
	initconline = getInitCon();
    errorMessage = "The input referenced has already been connected.";
}

int inputPrevConnected::getInitCon(void)
{
	int l;
	l = netz->getLineDefd(devicelink, ipid);
	return l;
}
	
void inputPrevConnected::getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition)
{
	l = line;
	c = col; 
    hasPosition = hasPos; 
	makeLongErrMsg();
	errmsg = longErrMsg;
	return;
}

void inputPrevConnected::makeLongErrMsg(void)
{
	string firstLine;
	int a = initconline;
	stringstream ss;
	ss << a;
	firstLine = ss.str();
	longErrMsg = errorMessage + firstLine; 
	return;
}
	
nameAlreadyDefd::nameAlreadyDefd(int l, int c, namestring_t dev, names* names_mod)
{
	errorMessage = "This device name has already been defined. ";
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

void nameAlreadyDefd::getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition)
{
	l = line;
	c = col; 
    hasPosition = hasPos;
	makeLongErrMsg();
	errmsg = longErrMsg;
	return;
}

void nameAlreadyDefd::makeLongErrMsg(void)
{
	string firstLine, firstCol;
	int a = initdefline;
	stringstream ss;
	ss << a;
	firstLine = ss.str();
	a = initdefcol;
	ss << a;
	firstCol = ss.str();
	longErrMsg = errorMessage + firstLine + ", " + firstCol; 
	return;
}
	

