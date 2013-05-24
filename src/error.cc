#include "error.h"
#include <iostream>
#include <string>

using namespace std; 


/**************************************************************************/
/* Methods for the superclass Error  */ 

/* Prints out the error message and line where the error occurred */ 
void Error::printErrMsg(void) 
{
   cout << "Error: " << line << ": " << errorMessage <<endl;
} 

Error::Error()
{
	return;
}

/******************************************************************************/
/* Methods for subclasses (ie. specific errors)   */ 

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


