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

void Warning::getWarningDetails(int &l, int &c, string &warnmsg, bool &hasPosition)
{
    l = line; 
    c = col; 
    warnmsg = "Warning: " + warningMessage;
    hasPosition = hasPos;
    return;
}

Warning::Warning()
{
    hasPos = true; 
    return;
}


/**************************************************************************/
/***************** methods for specific warnings **************************/
/**************************************************************************/

redefCon::redefCon(int l, int c)
{
    line = l; 
    col = c;
    warningMessage = "This connection has already been defined - solve by deleting this line";
}

nonClkInput::nonClkInput(int l, int c)
{ 
    line = l; 
    col = c;
    warningMessage = "Connecting a device type other than CLOCK to a dtype CLK input."; 
}


