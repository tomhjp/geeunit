#include <iostream>
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

void parser::devSectCheck(symbol_t symbol)
{
	if(symbol.symboltype == endsym)
	{
		errorvector.push_back(new noSemiCol(symbol.line, symbol.col));
		needskeyflag = 1;
	}	
	else if(symbol.symboltype != semicolsym)
	{
		context.push_back(symbol);
		return;
	}
	else 
	{
		bool pass = checkDevLine();
		if(pass); //make the device (look in devices class) 
		return;
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
	if(context[1].symboltype != equalsym)
	{
		errorvector.push_back(new expEqualSym(context[1].line, context[1].col));
		return false;
	}
	if(context[2].symboltype == (switchsym || andsym || nandsym || orsym || norsym))
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
			// number passed as parameter
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
	else if(context[2].symboltype == (dtypesym || xorsym))
	{
		if(context[3].symboltype != semicolsym)
		{
			errorvector.push_back(new expSemiColSym(context[3].line, context[3].col));
			return false;
		}
	}
	else
	{
	}
}

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
    else if(section == devsect) devSectCheck(symbol); 
    else if(section == monsect) monSectCheck(symbol);
    else if(section == consect) conSectCheck(symbol);
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

