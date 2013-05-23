#include "error.h"
#include <iostream>
#include <string>

using namespace std; 




/**************************************************************************/
/* Methods for the superclass Error  */ 

/* Sets the linecalled variable to the number passed */
void Error::setLine(int a)
{
    lineCalled = a; 
}

/* Prints out the error message and line where the error occurred */ 
void Error::printErrMsg(void) 
{
   cout << "Error: " << lineCalled << ": " << errorMessage <<endl;
} 


/******************************************************************************/
/* Methods for subclasses (ie. specific errors)   */ 

Example::Example(void) 
{
    errorMessage = "The specific error message for this kind of error";
}
