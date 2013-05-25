#ifndef error_h
#define error_h

#include <string>
#include "typedefs.h"
#include "names.h"


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

/********************************************************/
/****************   "Simple" errors *********************/

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

class expDevName : public Error
{
	public: 
	expDevName(int l, int c);
};

class expEqualSym : public Error
{
	public:
	expEqualSym(int l, int c);
};

class expOPSym : public Error
{
	public:
	expOPSym(int l, int c);
};

class expNumSym: public Error
{
	public:
	expNumSym(int l, int c);
};

class expDevTypeSym: public Error
{
	public:
	expDevTypeSym(int l, int c);
};

class paramRangeErrSwitch: public Error
{
	public:
	paramRangeErrSwitch(int l, int c);
};

class paramRangeErrGate: public Error
{
	public:
	paramRangeErrGate(int l, int c);
};

class paramRangeErrClk: public Error
{
	public;
	paramRangeErrClk(int l, int c);
};

class expCPSym : public Error
{
	public:
	expCPSym(int l, int c);
};
	
class expSemiColSym: public Error
{
	public:
	expSemiColSym(int l, int c);
};	
	
	

/***********************************************************/
/********* more complex errors to report *******************/
/***********************************************************/

class nameAlreadyDefd : public Error
{
	int initdefline;
	int initdefcol;
	namestring_t devname; 
	void getInitDef(void);
	names* nmz; 
	public:
	nameAlreadyDefd(int l, int c, namestring_t dev, names* names_mod);
	void printErrMsg(void);
};	
	
#endif










