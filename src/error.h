#ifndef error_h
#define error_h

#include <string>

using namespace std; 

typedef enum{noStrtFile, expDevSym} errorcode_t;

class Error 
{
  protected:  
	int lineCalled; 
	string errorMessage; 
  public: 
	void printErrMsg(void); 
	void setLine(int a);
    Error(void);
}; 

class Example : Error
{
    public:
    Example(void);
};



#endif
