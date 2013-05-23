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


/******************************************************************************/
/* Methods for subclasses (ie. specific errors)   */ 

noStrtFile::noStrtFile(int l, int c) 
{
    errorMessage = "Expected 'STARTFILE' keyword";
    line = l;
    col = c;
}
