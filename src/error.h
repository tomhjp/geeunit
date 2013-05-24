#ifndef error_h
#define error_h

#include <string>

using namespace std; 

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

class noStrtFile : public Error
{
    public:
    noStrtFile(int l, int c);
};

class expDeviSym : public Error
{
    public:
    expDeviSym(int l, int c);
};

class expMonSym : public Error
{
    public:
    expMonSym(int l, int c);
};

class expConSym : public Error
{
    public:
    expConSym(int l, int c);
};

class expEndFSym : public Error
{
    public:
    expEndFSym(int l, int c);
};

class noSemiCol : public Error
{
	public:
	noSemiCol(int l, int c);
}; 


#endif
