#include "warning.h"
#include "names.h"
#include <iostream>
#include <string>

using namespace std; 

/**************************************************************************/
/* Methods for the superclass Warning */ 

/* Prints out the warning message and line where the warning occurred */ 
void Warning::printWarnMsg(void) 
{
   cout << "Warning: " << line << ", " << col << ": " <<warningMessage <<endl;
} 

Warning::Warning()
{
	return;
}
