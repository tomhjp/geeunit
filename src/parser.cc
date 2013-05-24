#include <iostream>
#include "scanner.h"
#include "parser.h"
#include "error.h"

using namespace std;

/* The parser for the circuit definition files */

void parser::preStartFCheck(symbol_t symbol)
{
    if(symbol.symboltype != startfsym) 
    {
       // errorvector.push_back(new noStrtFile(symbol.line, symbol.col));
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
	else ;
}
/* Finds the next expected keyword after a keyword  */ 
/* only called if the needskeyfalg = 1 */ 
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
		monitor* monitor_mod, scanner_t* scanner_mod)
{
    netz = network_mod;  /* make internal copies of these class pointers */
    dmz = devices_mod;   /* so we can call functions from these classes  */
    mmz = monitor_mod;   /* eg. to call makeconnection from the network  */
    smz = scanner_mod;   /* class you say:                               */
                         /* netz->makeconnection (i1, i2, o1, o2, ok);   */

    /* any other initialisation you want to do? */
    section = prestartfsect; 
    needskeyflag = 0;
    skipflag = 0;
    nodevsymflag = 0;
}

