#ifndef error_h
#define error_h

#include <string>
#include "typedefs.h"
#include "names.h"
#include "network.h"


using namespace std;

class Error
{
  protected:
    bool hasPos;
    int line;
    int col;
    string errorMessage;
  public:
    void printErrMsg(void);
    void getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition);
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
    public:
    paramRangeErrClk(int l, int c);
};

class param1RangeErrSigGen: public Error
{
    public:
    param1RangeErrSigGen(int l, int c);
};

class paramNRangeErrSigGen: public Error
{
    public:
    paramNRangeErrSigGen(int l, int c);
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

class expCommaSym: public Error
{
    public:
    expCommaSym(int l, int c);
};

class unExpEndSym: public Error
{
    public:
    unExpEndSym(int l, int c);
};

class devNameUndef: public Error
{
    public:
    devNameUndef(int l, int c);
};

class expConnPuncSym: public Error
{
    public:
    expConnPuncSym(int l, int c);
};

class expDotSym: public Error
{
    public:
    expDotSym(int l, int c);
};

class expDtypeInput: public Error
{
    public:
    expDtypeInput(int l, int c);
};

class expDtypeOutput: public Error
{
    public:
    expDtypeOutput(int l, int c);
};

class expInputSym: public Error
{
    public:
    expInputSym(int l, int c);
};

class invInputSym: public Error
{
    public:
    invInputSym(int l, int c);
};

class inputUnDefd: public Error
{
    public:
    inputUnDefd(int l, int c);
};

class unconnectInp: public Error
{ 
  public:
    unconnectInp(string list);
};

class badDevType: public Error
{
    public:
    badDevType(int l, int c);
};

class lineBuildFailed: public Error
{
    public:
    lineBuildFailed(int l, int c);
};

class foundSymAfterEndf: public Error
{
    public:
    foundSymAfterEndf(int l, int c);
};

class overMaxMonLimit: public Error
{
    public:
    overMaxMonLimit(int l, int c);
};

class fileNotComplete: public Error
{
    public:
    fileNotComplete();
};

class fatalErr: public Error
{
    public:
    fatalErr(int l, int c);
};

/***********************************************************/
/********* more complex errors to report *******************/
/***********************************************************/


class inputPrevConnected: public Error
{
    int initconline;
    int getInitCon(void);
    void makeLongErrMsg(void);
    network* netz;
    devlink devicelink;
    name_t ipid;
    string longErrMsg;
  public:
    inputPrevConnected(int l, int c, name_t id, devlink dev, network* network_mod);
    void getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition);
    void printErrMsg(void);
};

class nameAlreadyDefd : public Error
{
    int initdefline;
    int initdefcol;
    void getInitDef(void);
    void makeLongErrMsg(void);
    namestring_t devname;
    names* nmz;
    string longErrMsg;
  public:
    nameAlreadyDefd(int l, int c, namestring_t dev, names* names_mod);
    void getErrorDetails(int &l, int &c, string &errmsg, bool &hasPosition);
    void printErrMsg(void);
};

#endif










