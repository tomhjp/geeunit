#include <iostream>
#include <cstdlib>
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "warning.h"
#include "names.h"
#include "network.h"
#include "device.h"

using namespace std;

/* The parser for the circuit definition files */

void parser::skipToBreak(symbol_t symbol)
{
	if(symbol.symboltype == semicolsym)
	{
	    skipflag = 0; 
	    return;
	}
	else if(symbol.symboltype == endsym)
	{
	    needskeyflag == 1;
	    skipflag = 0;
	    return;
	}
	else 
	    return; 
}

void parser::preStartFCheck(symbol_t symbol)
{
    if(symbol.symboltype != startfsym) 
    {
        errorvector.push_back(new noStrtFile(symbol.line, symbol.col));
        skipflag=1; 
    }
    else needskeyflag=1;
    return;
}

void parser::postEndFCheck(symbol_t symbol)
{
    errorvector.push_back(new foundSymAfterEndf(symbol.line, symbol.col));
    return;
}

/* Reads in symbols and builds up a line in the DEVICES, MONITORS and CONNECTIONS   */
/* sections of the input file.  Calls the appropriate syntax and semantic check     */
/* functions                                                                        */
void parser::mainLineBuild(symbol_t symbol)
{
    /* an END symbol has been found which does not immediately proceed a semicolon  */
    if(symbol.symboltype == endsym && context.size()!= 1)
    { 
        errorvector.push_back(new unExpEndSym(symbol.line, symbol.col));
	needskeyflag = 1;
        return;
    }
    /* The END symbol has been found in it's expected position - move on the section */
    else if(symbol.symboltype == endsym && context.size() == 1)
    {
	cout << "Found END correctly" <<endl;
        needskeyflag = 1;
	emptyContextVector();
        return;
    }    
    else if(symbol.symboltype != semicolsym)
    {
	    context.push_back(symbol);
	    return;
    }
    else 
    {
	cout <<section << endl;
	bool done;
	if(section == devsect)
	{
	    bool pass = checkDevLine();
	    if(pass && errorvector.size()==0)
	    {
		//make the device (look in devices class) 
		done = makeDevLine();        
		if(!done)
		{
		    errorvector.push_back(new lineBuildFailed(context[0].line, 0));
		    emptyContextVector();
		    return;
		}
		return;
	    }
	    else 
	    {
		skipflag = 1;
		emptyContextVector();
		return;
	    } 
	}
	else if(section == consect)
	{
	    bool pass = checkConLine();
	    if(pass && errorvector.size()==0)
	    {
		// make the connection
		done = makeConLine();
		if(!done)
		{
		    errorvector.push_back(new lineBuildFailed(context[0].line, 0));
		    emptyContextVector();
		    return;
		}
		emptyContextVector();
		return; 
	    }
	    else if(!pass)
	    {
		/* The line has failed to build correctly */
		//skipflag = 1;
		emptyContextVector();
		return;
	    }
	    else 
	    {
		emptyContextVector();
		return;
	    }
	}
	else if(section == monsect)
	{
	    bool pass = checkMonLine();
	    if(pass && errorvector.size() == 0)
	    {
		// make the monitor
		done = makeMonLine();
		if(!done)
		{
		    errorvector.push_back(new lineBuildFailed(context[0].line, 0));
		    emptyContextVector();
		    return;
		}
		return;
	    }
	    else if(!pass)
	    {
		//skipflag = 1;
		emptyContextVector();
		return;
	    }
	    else
	    {
		emptyContextVector();
		return;
	    }
	}    
    }	
}

bool parser::makeMonLine(void)
{
    bool ok;
    name_t dev, outp;
    devicekind devkind;
    dev = nmz->lookup(context[0].namestring);
    devkind = dmz->devkind(dev);
    if(devkind != dtype)
    {
	/* The monitor is not monitoring a dtype output */ 
	outp = 0;  // other device types have only one output 
    }
    else
    {
	/* The monitor is monitoring a dtype output 	*/
	if(context[2].symboltype = qsym) outp = qpin;
	else if(context[2].symboltype = qbarsym) outp = qbarpin;
    }
    mmz->makemonitor(dev, outp, ok);
    return ok;
}
/* Carries out the connection construction after a line has been successfully parsed	*/
/* if the errorvector is empty								*/
bool parser::makeConLine(void)
{
    bool ok; 
    int line; 
    name_t idev, inp, odev, outp; 
    devicekind ipdevkind, opdevkind;
    
    line = context[0].line;
    odev = nmz->lookup(context[0].namestring);
    opdevkind = dmz->devkind(odev);
    if(opdevkind != dtype)
    {
		/* The output device is not a dtype */ 
		idev = nmz->lookup(context[2].namestring);
		ipdevkind = dmz->devkind(idev);
		devlink devicelink = netz->finddevice(idev);
		outp = 0;  			// These devices only have one output. 
		if(ipdevkind != dtype)
		{
			/* The input device is not a dtype */ 
			inplink inputlink = netz->findinput(devicelink, idev);
			inp = inputlink->id; 
		}
		else
		{
			/* The input device is a dtype */
			if(context[4].symboltype == ddatasym) inp = datapin; 
			else if(context[4].symboltype == clksym) inp = clkpin; 
			else if(context[4].symboltype == dclearsym) inp = clrpin;
			else if(context[4].symboltype == dsetsym) inp = setpin;
		}
    }
    else
    {
		/* The output device is a dtype */
		/* Q input is stored first, then the qbar input.  From examination of the makedtype function  */
		if(context[2].symboltype == qsym)
			outp = 0;
		else
			outp = 1; 
		idev = nmz->lookup(context[4].namestring);
		ipdevkind = dmz->devkind(idev);
		devlink devicelink = netz->finddevice(idev);
		if(ipdevkind != dtype)
		{
			/* The input device is not a dtype */ 
			inplink inputlink = netz->findinput(devicelink, idev);
			inp = inputlink->id; 
		}
		else
		{
			/* The input device is a dtype */
			if(context[6].symboltype == ddatasym) inp = datapin; 
			else if(context[6].symboltype == clksym) inp = clkpin; 
			else if(context[6].symboltype == dclearsym) inp = clrpin;
			else if(context[6].symboltype == dsetsym) inp = setpin;
		}
	}
     
     netz->makeconnection(idev, inp, odev, outp, line, ok);
     return ok; 
}

/* Carries out the device construction after a line has been successfully parsed 	*/
/* if the errorvector is empty. 							*/
bool parser::makeDevLine(void) 
{
    bool ok;  //used to check the makedevice method completes successfully 
    devicekind dkind; 
    symboltype_t devtype; 
    name_t id;
    int variant;
    
    devtype = context[2].symboltype;  
    id = nmz->lookup(context[0].namestring);
    
    if(devtype == (andsym || nandsym || orsym || norsym || switchsym || clksym))
    {
	/* The makedevice() method must be passed a parameter in the variant field */ 
	variant = context[4].num;
    }
	
    if(devtype == andsym) dkind = andgate;
    else if(devtype == nandsym) dkind = nandgate;
    else if(devtype == orsym) dkind = orgate;
    else if(devtype == norsym) dkind = norgate;
    else if(devtype == switchsym) dkind = aswitch; 
    else if(devtype == clksym) dkind = aclock;
    else if(devtype == dtypesym) dkind = dtype;
    else if(devtype == xorsym) dkind = xorgate;
    
    dmz->makedevice(dkind, id, variant, ok);
    return ok; 
}

/* Checks the syntax and semantics of lines in the DEVICES section 	*/ 
/* returns true if the line is syntactically and semantically correct 	*/ 
bool parser::checkDevLine(void)
{
    Dtype dtypedev;	// used to check validity of parameters passed from input file
    Clk clkdev; 	// slightly strange variable names becuase too many of these were c++ keywords
    Xor xorgate; 	
    Gate gate; 
    Switch switchdev; 
    
    cout << "check DEVICES line" << endl; 
    /* First sybol in the line must be a devicename which is currently undefined. */ 
    if(context[0].symboltype == strsym)
    {
	    // search namelist for a device with this name. 
	    name_t namedefd;
	    namedefd = nmz->cvtname(context[0].namestring); 
	    if(namedefd != blankname)
	    {
		    errorvector.push_back(new nameAlreadyDefd(context[0].line, context[0].col, context[0].namestring, nmz));
		    return false;
	    }	
    }
    else
    {
	    errorvector.push_back(new expDevName(context[0].line, context[0].col));
	    return false;
    }
    /* Second symbol must be '='  */
    if(context[1].symboltype != equalsym)
    {
	    errorvector.push_back(new expEqualSym(context[1].line, context[1].col));
	    return false;
    }
    /* Third symbol must be a devicetype. The syntax branches depending on which devictype is called */
    /* These devices take a single number as their parameter */  
    if(context[2].symboltype == (clksym || switchsym || andsym || nandsym || orsym || norsym))
    {
	    if(context[3].symboltype != opsym)
	    {
		    errorvector.push_back(new expOPSym(context[3].line, context[3].col));
		    return false; 
	    }
	    if(context[4].symboltype != numsym)
	    {
		    errorvector.push_back(new expNumSym(context[4].line, context[4].col));
		    return false;
	    }
	    else
	    {
		    // a number has been passed as parameter
		    // checks that the parameter passed is vaild
		    if(context[2].symboltype == switchsym)
		    {
			    if(!switchdev.paramInValidRange(context[4].num))
			    {
				    errorvector.push_back(new paramRangeErrSwitch(context[4].line, context[4].col));
				    return false; 
			    }	
		    }
		    else if(context[2].symboltype == (andsym || nandsym || orsym || norsym))
		    {
			    if(!gate.paramInValidRange(context[4].num))
			    {
				    errorvector.push_back(new paramRangeErrGate(context[4].line, context[4].col));
				    return false;
			    }
		    }
		    else if(context[2].symboltype == clksym)
		    {
			    if(!clkdev.paramInValidRange(context[4].num))
			    {
				    errorvector.push_back(new paramRangeErrClk(context[4].line, context[4].col));
				    return false;
			    }
		    }
	    }
	    if(context[5].symboltype != cpsym)
	    {
		    errorvector.push_back(new expCPSym(context[5].line, context[5].col));
		    return false;
	    }	
	    if(context[6].symboltype != semicolsym)
	    {
		    errorvector.push_back(new expSemiColSym(context[6].line, context[6].col));
		    return false;
	    }
    }
    /* These are devicetypes which take no parameters  */ 
    else if(context[2].symboltype == (dtypesym || xorsym))
    {
	    if(context[3].symboltype != semicolsym)
	    {
		    errorvector.push_back(new expSemiColSym(context[3].line, context[3].col));
		    return false;
	    }
    }
    /* the third symbol is not a devicetype */ 
    else if(context[2].symboltype != (switchsym || andsym || nandsym || orsym || norsym || dtypesym || xorsym || clksym))
    {
	    errorvector.push_back(new expDevTypeSym(context[2].line, context[2].col));
	    return false; 
    }
    /* THE LINE HAS PASSED ALL SEMANTIC AND SYNTACTIC CHEKCS AND MUST NOW BE BUILT */ 
    else
    {
	    return true;
    }
    
}
 
/* Checks the syntax and semantics of a line in the CONNECTIONS section */
bool parser::checkConLine(void)
{
    cout << "check CONNECTIONS line" << endl; 

    if(!isStrSym(context[0]))
    {
	errorvector.push_back(new expDevName(context[0].line, context[0].col));
	return false;
    }
    if(!devNameDefined(context[0]))
    {
	errorvector.push_back(new devNameUndef(context[0].line, context[0].col));
	return false;
    }
    /* check which devicetype is the output device on the line  */
    devicekind opdevkind, ipdevkind; 
    name_t opid, ipid;
    opid = nmz->cvtname(context[0].namestring);
    opdevkind = dmz->devkind(opid);
    
    if(opdevkind != dtype)
    {
	/* device output is not from a dtype device */ 
	if(!isConnPuncSym(context[1]))
	{
	    errorvector.push_back(new expConnPuncSym(context[1].line, context[2].col));
	    return false;
	}
	if(!isStrSym(context[2]))
	{
	    errorvector.push_back(new expDevName(context[2].line, context[2].col));
	    return false; 
	}
	if(!devNameDefined(context[2]))
	{
	    errorvector.push_back(new devNameUndef(context[2].line, context[2].col));
	    return false;
	}
	if(!isDotSym(context[3]))
	{
	    errorvector.push_back(new expDotSym(context[3].line, context[3].col));
	    return false;
	}
	if(!isStrSym(context[4]))
	{
	    errorvector.push_back(new invInputSym(context[4].line, context[4].col));
	    return false;
	}
	/* following depend on whether device input is into dtype or not */ 
	ipid = nmz->cvtname(context[2].namestring);
	ipdevkind = dmz->devkind(ipid);
	if(ipdevkind != dtype)
	{
	    /* input device is not a dtype device 	*/ 
	    /* ie. nondtype -> nondtype 		*/ 
	    if(!gateInputDefined(context[4]))
	    {
		errorvector.push_back(new inputUnDefd(context[4].line, context[4].col));
		return false;
	    }
	    if(!gateInputUnconnected(context[4]))
	    {
		name_t devid = nmz->cvtname(context[4].namestring); // gets the id for the device
		devlink devicelink = netz->finddevice(devid); 
		errorvector.push_back(new inputPrevConnected(context[4].line, context[4].col, ipid, devicelink, netz));
		return false;
	    }
	    if(!isSemiColSym(context[5]))
	    {
		errorvector.push_back(new expSemiColSym(context[5].line, context[5].col));
		return false;
	    }
	}
	else
	{
	    /* input device is a dtype device */ 
	    if(!isDtypeInput(context[4]))
	    {
		errorvector.push_back(new expDtypeInput(context[4].line, context[4].col));
		return false;
	    }
	    if(!dtypeInputUnconnected(context[4]))
	    {
		name_t devid = nmz->cvtname(context[4].namestring); // gets the id for the device
		devlink devicelink = netz->finddevice(devid); 
		errorvector.push_back(new inputPrevConnected(context[4].line, context[4].col, ipid, devicelink, netz));
		return false;
	    }
	    if(!isSemiColSym(context[5]))
	    {
		errorvector.push_back(new expSemiColSym(context[5].line, context[5].col));
		return false;
	    }
	}
    }
    else
    {
	/* output devicetype is a dtype device   	*/
	/* ie. nondtype -> dtype 			*/
	if(!isDotSym(context[1]))
	{
	    errorvector.push_back(new expDotSym(context[1].line, context[1].col));
	    return false;
	}
	if(!isStrSym(context[2]))
	{
	    errorvector.push_back(new expDtypeOutput(context[2].line, context[2].col));
	    return false;
	}
	if(!isDtypeOutput(context[2]))
	{
	    errorvector.push_back(new expDtypeOutput(context[2].line, context[2].col));
	    return false;
	}
	if(!isConnPuncSym(context[3]))
	{
	    errorvector.push_back(new expConnPuncSym(context[3].line, context[3].col));
	    return false; 
	}
	if(!isStrSym(context[4]))
	{
	    errorvector.push_back(new expDevName(context[4].line, context[4].col));
	    return false;
	}
	if(!devNameDefined(context[4]))
	{
	    errorvector.push_back(new devNameUndef(context[4].line, context[4].col));
	    return false;
	}
	if(!isDotSym(context[5]))
	{
	    errorvector.push_back(new expDotSym(context[5].line, context[5].col));
	    return false;
	}
	/* The following depend on whether the input device is a dtype or not */ 
	ipid = nmz->cvtname(context[4].namestring);
	ipdevkind = dmz->devkind(ipid);
	if(ipdevkind != dtype)
	{
	    /* Input device is not a dtype 	*/ 
	    /* ie. dtype -> nondtype 		*/
	    if(!isStrSym(context[6]))
	    {
		errorvector.push_back(new expInputSym(context[6].line, context[6].col));
		return false;
	    }
	    if(!gateInputDefined(context[6]))
	    {
		errorvector.push_back(new inputUnDefd(context[6].line, context[6].col));
		return false;
	    }
	    if(!gateInputUnconnected(context[6]))
	    {
		name_t devid = nmz->cvtname(context[6].namestring); // gets the id for the device
		devlink devicelink = netz->finddevice(devid); 
		errorvector.push_back(new inputPrevConnected(context[6].line, context[6].col, ipid, devicelink, netz));
		return false;
	    }
	    if(!isSemiColSym(context[7]))
	    {
		errorvector.push_back(new expSemiColSym(context[7].line, context[7].col));
		return false;
	    }
	}
	else
	{
	    /* Input device is a dtype 		*/
	    /* ie. dtype -> dtype 		*/
	    if(!isStrSym(context[6]))
	    {
		errorvector.push_back(new expDtypeInput(context[6].line, context[6].col));
		return false;
	    }
	    if(!isDtypeInput(context[7]))
	    {
		errorvector.push_back(new expDtypeInput(context[6].line, context[6].col));
		return false;
	    }
	    if(!dtypeInputUnconnected(context[6]))
	    {
		name_t devid = nmz->cvtname(context[6].namestring); // gets the id for the device
		devlink devicelink = netz->finddevice(devid); 
		errorvector.push_back(new inputPrevConnected(context[6].line, context[6].col, ipid, devicelink, netz));
		return false;
	    }
	    if(!isSemiColSym(context[7]))
	    {
		errorvector.push_back(new expSemiColSym(context[7].line, context[7].col));
		return false;
	    }
	}
    }
    
    /* Method only reaches here if every relevant check above has passed  */
    return true; 
}

/* Checks the syntax and semantics of a line in the MONITORS section 	*/
/* Returns true if the line is syntactically and semantically correct 	*/ 
bool parser::checkMonLine(void)
{
    cout << "Check MONITORS line" <<endl; 
    if(!isStrSym(context[0]))
    {
	errorvector.push_back(new expDevName(context[0].line, context[0].col));
	return false;
    }
    if(!devNameDefined(context[0]))
    {
	errorvector.push_back(new devNameUndef(context[0].line, context[0].col));
	return false;
    }
    /* The following depend on the device type being monitored, as	*/
    /* dtype devices have multiple outputs				*/
    devicekind opdevkind; 
    name_t opid;
    opid = nmz->cvtname(context[0].namestring);
    opdevkind = dmz->devkind(opid);
    if(opdevkind !=dtype)
    {
	/* Monitoring a non-dtype device output  */
	if(!isSemiColSym(context[1]))
	{
	    errorvector.push_back(new expSemiColSym(context[1].line, context[1].col));
	    return false;
	}
    }
    else
    {
	/* Monitoring a dtype device output 	*/ 
	if(!isDotSym(context[1]))
	{
	    errorvector.push_back(new expDotSym(context[1].line, context[1].col));
	    return false;
	}
	if(!isStrSym(context[2]))
	{
	    errorvector.push_back(new expDtypeOutput(context[2].line, context[2].col));
	    return false;
	}
	if(!isDtypeOutput(context[2]))
	{
	    errorvector.push_back(new expDtypeOutput(context[2].line, context[2].col));
	    return false;
	}
	if(!isSemiColSym(context[3]))
	{
	    errorvector.push_back(new expSemiColSym(context[3].line, context[3].col));
	    return false;
	}
    }
    
    /* Method reaches here only if the line is syntactically and semantically correct  */ 
    return true; 
	    
}

/* Empties the context vector ready for reuse 				*/
void parser::emptyContextVector(void)
{
    context.erase(context.begin(), context.end());
    cout << "Context size is " << context.size() <<endl;
    return;
}

bool parser::isStrSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == strsym)
	retval = true; 
    return retval; 
}

bool parser::isConnPuncSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == connpuncsym)
	retval = true; 
    return retval;
}

bool parser::isDotSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == dotsym)
	retval = true; 
    return retval;
}

bool parser::isSemiColSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == semicolsym)
	retval = true; 
    return retval; 
}

bool parser::isDtypeInput(symbol_t symbol)
{
    bool retval = false; 
    if(symbol.symboltype == (ddatasym || clksym || dsetsym || dclearsym))
	retval = true; 
    return retval; 
}

bool parser::isDtypeOutput(symbol_t symbol)
{
    bool retval = false; 
    if(symbol.symboltype == (qsym || qbarsym))
	retval = true; 
    return retval; 
}

bool parser::devNameDefined(symbol_t symbol)
{
    bool retval = false; 
    name_t namedefd;
    namedefd = nmz->cvtname(symbol.namestring); 
    if(namedefd != blankname)
	retval = true; 
    return retval;
}

bool parser::gateInputDefined(symbol_t symbol)
{
    bool retval = false;
    /* this section gets a link to the input referenced. */
    /* returns null if the input is undefined		*/
    string ipstring = symbol.namestring; 
    string strnum = ipstring.substr(1,ipstring.end()-ipstring.begin());	// erases the first character of the string ("I")
    name_t ipid = atoi(strnum.c_str());  //converts the input number referenced to type name_t;
    name_t devid = nmz->cvtname(symbol.namestring); // gets the id for the device
    devlink devicelink = netz->finddevice(devid); 
    inplink inputlink = netz->findinput(devicelink, ipid); //gets a link to the input referenced 
    /* checks that the input exists */ 
    if(inputlink != NULL)
	retval = true; 
    return retval; 
}    

bool parser::gateInputUnconnected(symbol_t symbol)
{    
    bool retval = false;
    /* this section gets a link to the input referenced. */
    /* returns null if the input is undefined		*/
    string ipstring = symbol.namestring; 
    string strnum = ipstring.substr(1,ipstring.end()-ipstring.begin());	// erases the first character of the string ("I")
    name_t ipid = atoi(strnum.c_str());  //converts the input number referenced to type name_t;
    name_t devid = nmz->cvtname(symbol.namestring); // gets the id for the device
    devlink devicelink = netz->finddevice(devid); 
    inplink inputlink = netz->findinput(devicelink, ipid);
    if(inputlink->connect == NULL)
	retval = true; 
    return retval;
}
 
bool parser::dtypeInputUnconnected(symbol_t symbol)
{
    bool retval = false; 
    name_t devid, ipid; 
    devid = nmz->cvtname(symbol.namestring); // gets the id for the device
    devlink devicelink = netz->finddevice(devid); 
    if(symbol.symboltype == ddatasym) ipid = datapin; 
    else if(symbol.symboltype == clksym) ipid = clkpin; 
    else if(symbol.symboltype == dclearsym) ipid = clrpin;
    else if(symbol.symboltype == dsetsym) ipid = setpin;
    inplink inputlink = netz->findinput(devicelink, ipid);
    if(inputlink->connect == NULL)
	retval = true; 
    return retval;
}

/* Finds the next expected keyword after a keyword  		*/ 
/* only called if the needskeyflag = 1 				*/ 
/* used to check that the next symbol after END or STARTFILE is correct  */ 
void parser::nextKeyWordCheck(symbol_t symbol)
{
    if(section==prestartfsect)
    {
        if((symbol.symboltype != devsym) && (nodevsymflag == 0))
        {	
            errorvector.push_back(new expDeviSym(symbol.line, symbol.col));
            nodevsymflag = 1; 
            return;
        }
        else if(symbol.symboltype != devsym && nodevsymflag == 1) 
            return;
        else
        {
            needskeyflag=0; 
            section=devsect;
            return; 
        }
    }
    else if(section==devsect)
    {
    	if(symbol.symboltype != connsym && noconsymflag == 0)
    	{
	    errorvector.push_back(new expConSym(symbol.line, symbol.col));
	    noconsymflag = 1;
	    return;
    	}                   
	else if(symbol.symboltype != connsym && noconsymflag == 1)
	    return;
	else
	{
	    needskeyflag=0;
	    section=consect;
	    return;
	}
    }
    else if(section==consect)
    {
    if(symbol.symboltype != monsym && nomonsymflag == 0)
    {
	errorvector.push_back(new expMonSym(symbol.line, symbol.col));
	nomonsymflag = 1;
	return;
    }                   
	else if(symbol.symboltype != monsym && nomonsymflag == 1)
	    return;
	else
	{
	    needskeyflag=0;
	    section=monsect;
	    return;
	}
    }
    else if(section==monsect)
    {
    if(symbol.symboltype != endfsym && noendfsymflag == 0)
    {
		errorvector.push_back(new expEndFSym(symbol.line, symbol.col));
		noendfsymflag = 1;
		return;
    }                   
	else if(symbol.symboltype != endfsym && noendfsymflag == 1)
	    return;
	else
	{
	    needskeyflag=0;
	    filenotcompleteflag = 0;		//	file has been terminated correctly 
	    section=postendfsect;
	    return;
	}
    }
}

	
/*****************************************************************************/
/****************** Public methods *******************************************/
/*****************************************************************************/
/*****************************************************************************/

/* read in symbols from the scanner, and calls the relevant parsing functions */ 
void parser::readin (symbol_t symbol)
{   
    //if(skipflag == 1) skipToBreak(symbol);
    if(needskeyflag == 1) nextKeyWordCheck(symbol); 
    else if(section == prestartfsect) preStartFCheck(symbol); 
    else if((section == devsect) || (section == consect )|| (section == monsect)) mainLineBuild(symbol); 
    else if(section == postendfsect) postEndFCheck(symbol);
    else 
    {
	/* USED FOR BUG FIXING */
	cout << "Erm, something's gone wrong" << endl; 	//PANIC. 
	cout << "Section = " << section <<endl; 
    }
       
}

vector<Error*> parser::getErrorVector(void)
{
    return errorvector; 
}

vector<Warning*> parser::getWarningVector(void)
{
	return warningvector;
}

parser::parser (network* network_mod, devices* devices_mod,
		monitor* monitor_mod, scanner_t* scanner_mod, names* names_mod)
{
    netz = network_mod;  /* make internal copies of these class pointers */
    dmz = devices_mod;   /* so we can call functions from these classes  */
    mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
    smz = scanner_mod;   /* class you say:                               */
    nmz = names_mod;      /* netz->makeconnection (i1, i2, o1, o2, ok);   */

    /* any other initialisation you want to do? */
    section = prestartfsect; 
    needskeyflag = 0;
    skipflag = 0;
    nodevsymflag = 0;
    noconsymflag = 0;
    nomonsymflag = 0;
    noendfsymflag = 0;
    filenotcompleteflag = 1;
}

