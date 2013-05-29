#ifndef warning_h
#define warning_h

#include <string>
#include "typedefs.h"
#include "names.h"


using namespace std; 

/*Parent Warning class. specific warnings inherit from this */ 
class Warning
{
  protected:  
	int line;
	int col; 
	string warningMessage; 
  public: 
	void printWarnMsg(void); 
	void getWarningDetails(int &l, int &c, string &warnmsg);
	Warning(void);
}; 


/*************************** specific warnings  ***************************/ 



#endif


