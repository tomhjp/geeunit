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
    Error(void);
}; 

class noStrtFile : Error
{
    public:
    noStrtFile(int l, int c);
};

class expDevSym : Error
{
    public:
    expDevSym(int l, int c);
};

class expMonSym : Error
{
    public:
    expMonSym(int l, int c);
};

class expConSym : Error
{
    public:
    expConSym(int l, int c);
};

class expEndFSym : Error
{
    public:
    expEndFSym(int l, int c);
};

#endif
