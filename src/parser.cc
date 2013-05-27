#include <iostream>
#include <cstdlib>
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "names.h"

using namespace std;

/* The parser for the circuit definition files */

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

/* Reads in symbols and builds up a line in the DEVICES, MONITORS and CONNECTIONS   */
/* sections of the input file.  Calls the appropriate syntax and semantic check     */
/* functions                                                                        */
void parser::mainLineBuild(symbol_t symbol)
{
    if(symbol.symboltype == endsym && context.size()!= 0)
    { 
        errorvector.push_back(new unExpEndSym(symbol.line, symbol.col));
        return;
    }
    /* The END symbol has been found in it's expected position - move on the section */
    else if(symbol.symboltype == endsym && context.size() == 0)
    {
        needskeyflag = 1;
        return;
    }    
	else if(symbol.symboltype != semicolsym)
	{
		context.push_back(symbol);
		return;
	}
	else 
	{
	    if(section == devsect)
	    {
		    bool pass = checkDevLine();
		    if(pass && errorvector.size()==0)
		    {
		        //make the device (look in devices class) 
		        makeDevLine();        
		        return;
	        }
	        else 
	        {
	            skipflag = 1;
	            return;
	        } 
	    }
	    else if(section == consect)
	    {
	        bool pass = checkConLine();
	        if(pass && errorvector.size()==0)
	        {
	            // make the connection
	            makeConLine();
	            return; 
	        }
	        else if(!pass)
	        {
	            skipflag = 1;
	            return;
	        }
		else 
		{
		    return;
		}
	    }
	    else if(section == monsect)
	    {
	        bool pass = checkMonLine();
	        if(pass && errorvector.size() == 0)
	        {
	            // make the monitor
	            makeMonLine();
	            return;
	        }
	        else if(!pass)
	        {
	            skipflag = 1;
	            return;
	        }
		else
		{
		    return;
		}
	    }    
	
	
	}	
}

/* Checks the syntax and semantics of lines in the DEVICES section */ 
/* returns true if the line is syntactically and semantically correct */ 
bool parser::checkDevLine(void)
{
    Dtype dtypedev;	// used to check validity of parameters passed from input file
    Clk clkdev; 	// slightly strange variable names becuase too many of these were c++ keywords
    Xor xorgate; 	
    Gate gate; 
    Switch switchdev; 
	
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

/* checks the syntax and semantics of a line in the CONNECTIONS section */ 
bool parser::checkConLine(void)
{
    /* Find the device type of the output device. cvtname returns "blankname" if the device isn't found */ 
    /* It will also return "blankname" if an incorrect symbol is in the context vector (ie. a 	*/
    /* syntax error) so we don't need to worry about that here.					*/
    devicekind opdevkind; 
    name_t opid = nmz->cvtname(context[0].namestring);
    opdevkind = dmz->devkind(opid);
    
    if(context[0].symboltype != strsym)
    {
	errorvector.push_back(new expDevName(context[0].line, context[0].col));
	return false;
    }
    if(opid == blankname)
    {
	errorvector.push_back(new devNameUndef(context[0].line, context[0].col));
	return false;
    }    

    /* Set of syntax and semantic checks for connections with DTYPE output devices  */
    if(opdevkind == dtype)
    {
	if(context[1].symboltype != dotsym)
	{
	    errorvector.push_back(new expDotSym(context[1].line, context[1].col));
	    return false; 
	}
	if(context[2].symboltype != (qsym || qbarsym))
	{
	    errorvector.push_back(new expDtypeInput(context[2].line, context[2].col));
	    return false;
	}
	if(context[3].symboltype != connpuncsym)
	{
	    errorvector.push_back(new expConnPuncSym(context[3].line, context[3].col));
	    return false;
	}
	
	/* Find the device type of the output device. cvtname returns "blankname" if the device isn't found */ 
	devicekind ipdevkind; 
	name_t ipid = nmz->cvtname(context[4].namestring);
	ipdevkind = dmz->devkind(ipid);
	
	if(ipid == blankname)
	{
	    errorvector.push_back(new devNameUndef(context[4].line, context[4].col));
	    return false;
	}    
	
	if(context[5].symboltype != dotsym)    
	{
	    errorvector.push_back(new expDotSym(context[5].line, context[5].col));
	    return false; 
	}    
	
	/* Set of syntax and semantic checks for connections with DTYPE input and output devices  */
	if(ipdevkind == dtype)
	{
	    if(context[6].symboltype != (ddatasym || clksym || dsetsym || dclearsym))
	    {
		errorvector.push_back(new expDtypeInput(context[6].line, context[6].col));
		return false;
	    }
	    if(context[7].symboltype != semicolsym)
	    {
		errorvector.push_back(new expSemiColSym(context[7].line, context[7].col));
		return false;
	    }
	    else
	    {
		return true; 
	    }
	}    
	/* Set of checks for connections with DTYPE output device and non-DTYPE input device */ 
	else
	{
	    if(context[6].symboltype != strsym)
	    {
		errorvector.push_back(new expInputSym(context[4].line, context[4].col));
		return false; 
	    }

  	    /*  extract the input being referenced from the namestring information */
	    namestring_t ipstring = context[6].namestring; 
	    (string)ipstring;
	    char *firstletter = (char*) ipstring[0]; 
	    string compstring = "I";
	    if(firstletter.compare(compstring) != 0)
	    {
		errorvector.push_back(new invInputSym(context[6].line, context[6].col));
		return false;
	    }
	    
	    int ipnum; 
	    ipstring.erase(0,1);	// erases the first character of the string ("I")
	    
	    for(int i=0; i<ipstring.length(); i++)
	    {
		if(!isdigit(ipstring[i]))
		{
		   errorvector.push_back(new invInputSym(context[6].line, context[6].col));
		    return false;
		} 
	    }
	    ipnum = atoi(ipstring);	// converts the string into an int. 
	    ipnum = (name_t) ipnum; 
	    
	    
	    
	    
	    
		
    if(context[1].symboltype != connpuncsym)
    {
	errorvector.push_back(new expConnPuncSym(context[1].line, context[1].col));
	return false;
    }
    if(context[2].symboltype != strsym)
    {
	errorvector.push_back(new expDevName(context[0].line, context[0].col));
	return false;
    }
    else
    {
	if(nmz->cvtname(context[0].namestring) == blankname)
	{
	    errorvector.push_back(new devNameUndef(context[0].line, context[0].col));
	    return false;
	}    
    }
    if(context[3].symboltype != dotsym)
    {
	errorvector.push_back(new expDotSym(context[3].line, context[3].col));
	return false; 
    }
    if(context[4].symboltype != strsym)
    {
	errorvector.push_back(new expInputSym(context[4].line, context[4].col));
	return false; 
    }
    
    //namestring_t ipstring = context[4].namestring;
    
    

/* Finds the next expected keyword after a keyword  */ 
/* only called if the needskeyflag = 1 				*/ 
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
    if(skipflag == 1) skipToBreak();
    else if(needskeyflag == 1) nextKeyWordCheck(symbol);
    else if(section == prestartfsect) preStartFCheck(symbol); 
    else if(section == (devsect || consect || monsect)) mainLineBuild(symbol); 
    else if(section == postendfsect) postEndFCheck(symbol);
    else cout << "Erm, something's gone wrong" << endl; //PANIC.        
}

vector<Error*> parser::getErrorVector(void)
{
    return errorvector; 
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
}

