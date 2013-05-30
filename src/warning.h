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
    bool hasPos;
  public: 
	void printWarnMsg(void); 
	void getWarningDetails(int &l, int &c, string &warnmsg, bool &hasPosition);
	Warning(void);
}; 


/*************************** specific warnings  ***************************/ 

class redefCon: public Warning
{
    public:
    redefCon(int l, int c);
};

class nonClkInput: nonClkInput
{
    public:
    nonClkInput(int l, int c);
};


#endif


