#ifndef error_h
#define error_h

#include <string>

using namespace std; 

typedef enum{noStrtFile, expDevSym} errorcode_t;

class Error 
{
  protected:  
	int line;
	int col; 
	string errorMessage; 
  public: 
	void printErrMsg(void); 
	void setLine(int a);
    Error(void);
}; 

class noStrtFile : Error
{
    public:
    noStrtFile(int l, int c);
};



#endif
