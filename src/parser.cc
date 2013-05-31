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

void parser::preStartFCheck(symbol_t symbol)
{
    if(symbol.symboltype != startfsym) 
    {
        errorvector.push_back(new noStrtFile(symbol.line, symbol.col)); 
    }
    else needskeyflag=1;
    return;
}

void parser::postEndFCheck(symbol_t symbol)
{
    if(symbol.symboltype != eofsym)
    errorvector.push_back(new foundSymAfterEndf(symbol.line, symbol.col));
    return;
}

/* Reads in symbols and builds up a line in the DEVICES, MONITORS and CONNECTIONS   */
/* sections of the input file.  Calls the appropriate syntax and semantic check     */
/* functions                                                                        */
void parser::mainLineBuild(symbol_t symbol)
{
    context.push_back(symbol);
    if(symbol.symboltype == endsym)
    { 
        /* an END symbol has been found */
        if(context.size() != 1)
        {
            /* The END symbol is not on a new line */     
            errorvector.push_back(new unExpEndSym(symbol.line, symbol.col));
            needskeyflag = 1;
            emptyContextVector();
            return;
        }
        else
        {
            /* The END symbol has been found in it's expected position - move on the section */
            if(section == consect)
            {
                bool allInsCon;
                netz->checknetwork(allInsCon);
                if(!allInsCon)
                {
                    // check all inputs to find those which are not connected 
                    errorvector.push_back(new unconnectInp());
                }
            }
            needskeyflag = 1;
            emptyContextVector();
            return;
        }    
    }
    
    else if(!isSemiColSym(symbol))
    {
        return;
    }
    else 
    {
        bool done;
        if(section == devsect)
        {
            bool pass = checkDevLine();
            if(pass)
            {
                //make the device 
                done = makeDevLine();        
                if(!done)
                {
                    errorvector.push_back(new lineBuildFailed(context[0].line, 0));
                    emptyContextVector();
                    return;
                }
                emptyContextVector();
                return;
            }
            else 
            {
                emptyContextVector();
                return;
            } 
        }
        else if(section == consect)
        {
            bool pass = checkConLine();
            if(pass)
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
            else 
            {
                emptyContextVector();
                return;
            }
        }
        else if(section == monsect)
        {
            bool pass = checkMonLine();
            if(pass)
            {
                // make the monitor
                done = makeMonLine();
                if(!done)
                {
                    errorvector.push_back(new lineBuildFailed(context[0].line, 0));
                    emptyContextVector();
                    return;
                }
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
    name_t dev, outp, opid;
    devicekind opdevkind;
    devlink opdevlink;
    
    if(nummonitorsmade < MAXMONITORS)
    {
	dev = nmz->lookup(context[0].namestring);
	opdevlink = netz->finddevice(dev);
	opdevkind = opdevlink->kind;
	if(opdevkind != dtype)
	{
	    /* The monitor is not monitoring a dtype output */ 
	    outp = blankname;  // these device types have only one output 
	}
	else
	{
	    /* The monitor is monitoring a dtype output     */
	    namestring_t opdevstring, opstring;
	    opstring = context[2].namestring;
	    opdevstring = context[0].namestring;
	    opid = nmz->cvtname(opstring);
	    devlink devicelink = netz->finddevice(dev); 
	    outplink outputlink = netz->findoutput(devicelink, opid);
	    outp = outputlink->id;
	}
	mmz->makemonitor(dev, outp, ok);
	nummonitorsmade++; 
    }
    else
    {
	errorvector.push_back(new overMaxMonLimit(context[0].line, context[0].col));
	ok = true;	// Overflow been handled correctly - do not register a parser error on return 
    }
    return ok;
}


/* Carries out the connection construction after a line has been successfully parsed    */
bool parser::makeConLine(void)
{
    bool ok; 
    int line; 
    name_t idev, inp, odev, outp, ipid; 
    devlink opdevlink, ipdevlink; 
    devicekind ipdevkind, opdevkind;
    namestring_t ipstring, opstring;
    
    line = context[0].line;
    odev = nmz->lookup(context[0].namestring);
    opdevlink = netz->finddevice(odev);
    opdevkind = opdevlink->kind;

    if(opdevkind != dtype)
    {
    /* The output device is not a dtype */ 
    
    idev = nmz->lookup(context[2].namestring);
    devlink ipdevlink = netz->finddevice(idev);
    ipdevkind = ipdevlink->kind;
    
    outp = blankname;           // These devices only have one output. 
    if(ipdevkind != dtype)
    {
        /* The input device is not a dtype  */ 
        /* ie. non-dtype -> non-dtype   */
        ipstring = context[4].namestring;
        name_t ipid = nmz->cvtname(ipstring);
        inplink inputlink = netz->findinput(ipdevlink, ipid);
        inp = inputlink->id; 
    }
    else
    {   
        /* The input device is a dtype  */
        /* ie non-dtype -> dtype            */
        ipstring = context[4].namestring;
        name_t ipid = nmz->cvtname(ipstring);
        if(context[4].symboltype == ddatasym) inp = nmz->cvtname("DATA"); 
        else if(context[4].symboltype == dclksym) inp = nmz->cvtname("CLK"); 
        else if(context[4].symboltype == dclearsym) inp = nmz->cvtname("CLEAR");
        else if(context[4].symboltype == dsetsym) inp = nmz->cvtname("SET");
    }
    }
    else
    {
        /* The output device is a dtype */
        name_t opdevid;
        namestring_t opdevstring;
        opstring = context[2].namestring;
        opdevstring = context[0].namestring;
        name_t opid = nmz->cvtname(opstring);
        opdevid = nmz->cvtname(opdevstring);
        devlink devicelink = netz->finddevice(opdevid); 
        outplink outputlink = netz->findoutput(devicelink, opid);
        outp = outputlink->id;
          
          
        idev = nmz->lookup(context[4].namestring);
        devlink ipdevlink = netz->finddevice(idev);
        ipdevkind = ipdevlink->kind;  

        if(ipdevkind != dtype)
        {
            /* The input device is not a dtype  */ 
            /* ie dtype -> non-dtype            */
            name_t ipid;
            ipstring = context[6].namestring;
            ipid = nmz->cvtname(ipstring);
            inplink inputlink = netz->findinput(ipdevlink, ipid);
            inp = inputlink->id; 
        }
        else
        {
            /* The input device is a dtype  */
            /* ie. dtype -> dtype           */

            if(context[6].symboltype == ddatasym) inp = nmz->cvtname("DATA"); 
            else if(context[6].symboltype == dclksym) inp = nmz->cvtname("CLK"); 
            else if(context[6].symboltype == dclearsym) inp = nmz->cvtname("CLEAR");
            else if(context[6].symboltype == dsetsym) inp = nmz->cvtname("SET");
        }
    }
 
    netz->makeconnection(idev, inp, odev, outp, line, ok);
    return ok; 
}

/* Carries out the device construction after a line has been successfully parsed    */
bool parser::makeDevLine(void) 
{
    bool ok;  //used to check the makedevice method completes successfully 
    devicekind dkind; 
    symboltype_t devtype; 
    name_t id;
    int variant;
     
    id = nmz->lookup(context[0].namestring);
    
    if((isAndSym(context[2])) || (isNandSym(context[2])) || (isOrSym(context[2])) || (isNorSym(context[2])) || (isSwitchSym(context[2])) || (isClkSym(context[2])))
    {
        /* If the device is a gate, clock, or switch                   */
        /* The makedevice() method must be passed a parameter in the variant field */ 
        variant = context[4].num;
    }
    
    if(isAndSym(context[2])) dkind = andgate;
    else if(isNandSym(context[2])) dkind = nandgate;
    else if(isOrSym(context[2])) dkind = orgate;
    else if(isNorSym(context[2])) dkind = norgate;
    else if(isSwitchSym(context[2])) dkind = aswitch; 
    else if(isClkSym(context[2])) dkind = aclock;  
    else if(isDtypeSym(context[2])) dkind = dtype;
    else if(isXorSym(context[2])) dkind = xorgate;
    
    dmz->makedevice(dkind, id, variant, ok);
    nmz->setPos(context[0].namestring, context[0].line, context[0].col);
    return ok; 
}

/* Checks the syntax and semantics of lines in the DEVICES section  */ 
/* returns true if the line is syntactically and semantically correct   */ 
bool parser::checkDevLine(void)
{
    /* creates objects from the device.cc file */ 
    Dtype dtypedev; // used to check validity of parameters passed from input file
    Clk clkdev;    
    Xor xorgate;    
    Gate gate; 
    Switch switchdev; 

    /* First symbol in the line must be a devicename which is currently undefined. */ 
    if(isStrSym(context[0]))
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
    if(!isEqualSym(context[1]))
    {
        errorvector.push_back(new expEqualSym(context[1].line, context[1].col));
        return false;
    }
    /* Third symbol must be a devicetype. The syntax branches depending on which devicetype is called */
    /* These devices take a single number as their parameter */  
    if((isClkSym(context[2])) || (isSwitchSym(context[2])) || (isAndSym(context[2])) || (isNandSym(context[2])) || (isOrSym(context[2])) || (isNorSym(context[2])))
    {    
        if(!isOpSym(context[3]))
        {
            errorvector.push_back(new expOPSym(context[3].line, context[3].col));
            return false; 
        }
        if(!isNumSym(context[4]))
        {
            errorvector.push_back(new expNumSym(context[4].line, context[4].col));
            return false;
        }
        else
        {
            // a number has been passed as parameter
            // checks that the parameter passed is vaild
            if(isSwitchSym(context[2]))
            {
                if(!switchdev.paramInValidRange(context[4].num))
                {
                    errorvector.push_back(new paramRangeErrSwitch(context[4].line, context[4].col));
                    return false; 
                }   
            }
            else if((isAndSym(context[2])) || (isNandSym(context[2])) || (isOrSym(context[2])) || (isNorSym(context[2])))
            {
                if(!gate.paramInValidRange(context[4].num))
                {
                    errorvector.push_back(new paramRangeErrGate(context[4].line, context[4].col));
                    return false;
                }
            }
            else if(isClkSym(context[2]))
            {
                if(!clkdev.paramInValidRange(context[4].num))
                {
                    errorvector.push_back(new paramRangeErrClk(context[4].line, context[4].col));
                    return false;
                }
            }
        }
        if(!isCpSym(context[5]))
        {
            errorvector.push_back(new expCPSym(context[5].line, context[5].col));
            return false;
        }   
        if(!isSemiColSym(context[6]))
        {
	    // reports the column where the ';' was expected, not where the error was detected 
	    int col = context[5].col + context[5].namestring.length();		
            errorvector.push_back(new expSemiColSym(context[5].line, col));
            return false;
        }
    }
    /* These are devicetypes which take no parameters  */ 
    else if(isDtypeSym(context[2]) || (isXorSym(context[2])))
    {
        if(!isSemiColSym(context[3]))
        {
	    // reports the column where the ';' was expected, not where the error was detected 
	    int col = context[2].col + context[2].namestring.length();		
            errorvector.push_back(new expSemiColSym(context[2].line, col));
            return false;
        }
    }
    /* the third symbol is not a devicetype */ 
    else if((!isSwitchSym(context[2])) || (!isAndSym(context[2])) || (!isNandSym(context[2])) || (!isOrSym(context[2])) || (!isNorSym(context[2])) || (!isDtypeSym(context[2])) || (!isXorSym(context[2])) || (!isClkSym(context[2])))
    {
        errorvector.push_back(new expDevTypeSym(context[2].line, context[2].col));
        return false; 
    }
    
    /* The line has passed all semantic and syntactic checks and should now be built */ 
    return true;
    
}
 
/* Checks the syntax and semantics of a line in the CONNECTIONS section */
bool parser::checkConLine(void)
{
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
    devlink opdevlink, ipdevlink; 
    
    opid = nmz->cvtname(context[0].namestring);
    opdevlink = netz->finddevice(opid);
    opdevkind = opdevlink->kind;

    if(opdevkind == baddevice)
    {
        errorvector.push_back(new badDevType(context[0].line, context[0].col));
        return false;
    }
    else if(opdevkind != dtype)
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

        /* the following depend on whether the device input is from a dtype or not */ 

        ipid = nmz->cvtname(context[2].namestring);
        ipdevlink = netz->finddevice(ipid);
        ipdevkind = ipdevlink->kind;

        if(ipdevkind != dtype)
        {
            /* input device is not a dtype device   */ 
            /* ie. nondtype -> nondtype             */ 
            name_t devid = nmz->cvtname(context[2].namestring); // gets the id for the device
            if(!isStrSym(context[4]))
            {
                errorvector.push_back(new invInputSym(context[4].line, context[4].col));
                return false;
            }
            if(!gateInputDefined(context[4], devid))
            {
                errorvector.push_back(new inputUnDefd(context[4].line, context[4].col));
                return false;
            }
            else
            {
                if(!gateInputUnconnected(context[4], devid))
                {
		    if(connectRedefined(context[2], context[4], context[0], context[0]))
		    {
			warningvector.push_back(new redefCon(context[0].line, context[0].col));
		    }
		    else
		    {
			devlink devicelink = netz->finddevice(devid); 
			name_t inputid = nmz->cvtname(context[4].namestring);
			errorvector.push_back(new inputPrevConnected(context[4].line, context[4].col, inputid, devicelink, netz));
			return false;
		    }
                }
            }
            if(!isSemiColSym(context[5]))
            {
                // reports the column where the ';' was expected, not where the error was detected 
		int col = context[4].col + context[4].namestring.length();		
		errorvector.push_back(new expSemiColSym(context[4].line, col));
                return false;
            }
        }
        else
        {
            /* input device is a dtype device   */ 
            /* ie. non-dtype -> dtype       */
            if(!isDtypeInput(context[4]))
            {
                errorvector.push_back(new expDtypeInput(context[4].line, context[4].col));
                return false;
            }
            if(!dtypeInputUnconnected(context[2], context[4]))
            {
                name_t devid = nmz->cvtname(context[2].namestring); // gets the id for the device
                devlink devicelink = netz->finddevice(devid); 
		name_t inputid = nmz->cvtname(context[4].namestring);
                errorvector.push_back(new inputPrevConnected(context[4].line, context[4].col, inputid, devicelink, netz));
                return false;
            }
            if(!isSemiColSym(context[5]))
            {
                // reports the column where the ';' was expected, not where the error was detected 
		int col = context[4].col + context[4].namestring.length();		
		errorvector.push_back(new expSemiColSym(context[4].line, col));
                return false;
            }
            
        }
    }
    else
    {
        /* output devicetype is a dtype device      */
        if(!isDotSym(context[1]))
        {
            errorvector.push_back(new expDotSym(context[1].line, context[1].col));
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
        ipdevlink = netz->finddevice(ipid);
        ipdevkind = ipdevlink->kind;
        if(ipdevkind != dtype)
        {
            /* Input device is not a dtype  */ 
            /* ie. dtype -> nondtype        */
            name_t devid = nmz->cvtname(context[4].namestring); // gets the id for the device
            if(!isStrSym(context[6]))
            {
                errorvector.push_back(new expInputSym(context[6].line, context[6].col));
                return false;
            }
            if(!gateInputDefined(context[6], devid))
            {
                errorvector.push_back(new inputUnDefd(context[6].line, context[6].col));
                return false;
            }
            if(!gateInputUnconnected(context[6], devid))
            {
                devlink devicelink = netz->finddevice(devid); 
		name_t inputid = nmz->cvtname(context[6].namestring);
                errorvector.push_back(new inputPrevConnected(context[6].line, context[6].col, inputid, devicelink, netz));
                return false;
            }
            if(!isSemiColSym(context[7]))
            {
                // reports the column where the ';' was expected, not where the error was detected 
		int col = context[6].col + context[6].namestring.length();		
		errorvector.push_back(new expSemiColSym(context[6].line, col));
                return false;
            }
        }
        else
        {
            /* Input device is a dtype      */
            /* ie. dtype -> dtype       */
            if(!isDtypeInput(context[6]))
            {
                errorvector.push_back(new expDtypeInput(context[6].line, context[6].col));
                return false;
            }
            if(!dtypeInputUnconnected(context[4], context[6]))
            {
                name_t devid = nmz->cvtname(context[4].namestring); // gets the id for the device
                devlink devicelink = netz->finddevice(devid); 
		name_t inputid = nmz->cvtname(context[4].namestring);;
                errorvector.push_back(new inputPrevConnected(context[6].line, context[6].col, inputid, devicelink, netz));
                return false;
            }
            if(!isSemiColSym(context[7]))
            {
                // reports the column where the ';' was expected, not where the error was detected 
		int col = context[6].col + context[6].namestring.length();		
		errorvector.push_back(new expSemiColSym(context[6].line, col));
                return false;
            }
	    if(context[6].symboltype == dclksym)
	    {
		warningvector.push_back(new nonClkInput(context[0].line, context[0].col));
	    }
        }
    }
    
    /* The line has passed all syntactic and semantic checks and should now be built  */
    return true; 
}

/* Checks the syntax and semantics of a line in the MONITORS section    */
/* Returns true if the line is syntactically and semantically correct   */ 
bool parser::checkMonLine(void)
{    
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
    /* The following depend on the device type being monitored, as  */
    /* dtype devices have multiple outputs              */
    devicekind opdevkind; 
    name_t opid;
    devlink opdevlink; 
    
    opid = nmz->cvtname(context[0].namestring);
    opdevlink = netz->finddevice(opid);
    opdevkind = opdevlink->kind;

    if(opdevkind == baddevice)
    {
        errorvector.push_back(new badDevType(context[0].line, context[0].col));
        return false;
    }
    else if(opdevkind !=dtype)
    {
        /* Monitoring a non-dtype device output  */
        if(!isSemiColSym(context[1]))
        {
	    // reports the column where the ';' was expected, not where the error was detected 
	    int col = context[0].col + context[0].namestring.length();		
	    errorvector.push_back(new expSemiColSym(context[0].line, col));
            return false;
        }
    }
    else
    {
        /* Monitoring a dtype device output     */ 
        if(!isDotSym(context[1]))
        {
            errorvector.push_back(new expDotSym(context[1].line, context[1].col));
            return false;
        }
        if(!isDtypeOutput(context[2]))
        {
            errorvector.push_back(new expDtypeOutput(context[2].line, context[2].col));
            return false;
        }
        if(!isSemiColSym(context[3]))
        {
            // reports the column where the ';' was expected, not where the error was detected 
	    int col = context[2].col + context[2].namestring.length();		
	    errorvector.push_back(new expSemiColSym(context[2].line, col));
            return false;
        }
    }
    
    /* The line is syntactically and semantically correct and should now be built  */ 
    return true; 
        
}

/* Empties the context vector ready for reuse               */
void parser::emptyContextVector(void)
{
    context.erase(context.begin(), context.end());
    return;
}

bool parser::isClkSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == clksym)
        retval = true; 
    return retval;
}

bool parser::isAndSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == andsym)
        retval = true; 
    return retval;
}

bool parser::isNandSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == nandsym)
        retval = true; 
    return retval;
}

bool parser::isOrSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == orsym)
        retval = true; 
    return retval;
}

bool parser::isNorSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == norsym)
        retval = true; 
    return retval;
}

bool parser::isXorSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == xorsym)
        retval = true; 
    return retval;
}

bool parser::isSwitchSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == switchsym)
        retval = true; 
    return retval;
}

bool parser::isDtypeSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == dtypesym)
        retval = true; 
    return retval;
}

bool parser::isEqualSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == equalsym)
        retval = true; 
    return retval;
}

bool parser::isOpSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == opsym)
        retval = true; 
    return retval;
}

bool parser::isCpSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == cpsym)
        retval = true; 
    return retval;
}

bool parser::isDevSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == devsym)
        retval = true;
    return retval; 
}    

bool parser::isStrSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == strsym)
        retval = true; 
    return retval; 
}

bool parser::isNumSym(symbol_t symbol)
{
    bool retval = false;
    if(symbol.symboltype == numsym)
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
    if((symbol.symboltype == ddatasym) || (symbol.symboltype == dclksym) || (symbol.symboltype == dsetsym) || (symbol.symboltype == dclearsym))
        retval = true; 
    return retval; 
}

bool parser::isDtypeOutput(symbol_t symbol)
{
    bool retval = false; 
    if((symbol.symboltype == qsym) || (symbol.symboltype == qbarsym))
        retval = true; 
    return retval; 
}

bool parser::devNameDefined(symbol_t symbol)
{
    bool retval = true; 
    name_t namedefd; 
    namedefd = nmz->cvtname(symbol.namestring); 
    if(namedefd == blankname)
        retval = false; 
    return retval;
}

bool parser::gateInputDefined(symbol_t symbol, name_t devid)
{
    bool retval = false;
    namestring_t ipstring = symbol.namestring;
    name_t ipid = nmz->cvtname(ipstring);
    devlink devicelink = netz->finddevice(devid); 
    inplink inputlink = netz->findinput(devicelink, ipid); //gets a link to the input referenced 
    /* checks that the input exists. returns NULL if the input is not found */ 
    if(inputlink != NULL)
        retval = true; 
    return retval; 
}    

bool parser::gateInputUnconnected(symbol_t symbol, name_t devid)
{    
    bool retval = false;
    namestring_t ipstring = symbol.namestring;
    name_t ipid = nmz->cvtname(ipstring);
    devlink devicelink = netz->finddevice(devid); 
    inplink inputlink = netz->findinput(devicelink, ipid);
    if(inputlink->connect == NULL)
        retval = true;      
    return retval;
 
}

bool parser::connectRedefined(symbol_t idevsymbol, symbol_t ipsymbol, symbol_t odevsymbol, symbol_t opsymbol)
{    
    /* tests whether the connection is the ame as that already stored  */ 
    bool retval = false;
  
    name_t idevid = nmz->cvtname(idevsymbol.namestring); 
    devlink idevlink = netz->finddevice(idevid);
    name_t ipid = nmz->cvtname(ipsymbol.namestring); 
    inplink incon = netz->findinput(idevlink, ipid);
    outplink con = incon->connect;
    name_t iconid = con->id;
    
    name_t odevid = nmz->cvtname(odevsymbol.namestring);
    devlink odevlink = netz->finddevice(odevid);
   
    name_t opid;
    if(odevlink->kind ==dtype) opid = nmz->cvtname(opsymbol.namestring); 
    else 
	opid = blankname;

    outplink outcon = netz->findoutput(odevlink, opid);
    name_t oconid = outcon->id;

    if(iconid == oconid)
	retval = true;
	
    return retval;
}
 
bool parser::dtypeInputUnconnected(symbol_t dtypename, symbol_t dtypeinput)
{
 
    bool retval = false; 
    name_t devid, ipid; 
    devid = nmz->cvtname(dtypename.namestring); // gets the id for the device
    devlink devicelink = netz->finddevice(devid); 
    if (dtypeinput.symboltype == ddatasym) ipid = nmz->cvtname("DATA");
    else if (dtypeinput.symboltype == dclksym) ipid = nmz->cvtname("CLK");
    else if (dtypeinput.symboltype == dclearsym) ipid = nmz->cvtname("CLEAR");
    else if (dtypeinput.symboltype == dsetsym) ipid = nmz->cvtname("SET");  
    inplink inputlink = netz->findinput(devicelink, ipid);
    if(inputlink->connect == NULL)
        retval = true; 
    return retval;
}

/* Finds the next expected keyword after a keyword          */ 
/* only called if the needskeyflag = 1              */ 
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
	    filenotcompleteflag = 0;        //file has been terminated correctly with ENDFILE
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
    /* Calls the methods relevant to the section being parsed currently */
    if(symbol.symboltype == eofsym)
    {
	if(filenotcompleteflag == 1)
	{
	    /* The file has terminated prematurely */ 
	    errorvector.push_back(new fileNotComplete());
	    return;
	}
    }
    if(needskeyflag == 1) nextKeyWordCheck(symbol); 
    else if(section == prestartfsect) preStartFCheck(symbol); 
    else if((section == devsect) || (section == consect )|| (section == monsect)) mainLineBuild(symbol); 
    else if(section == postendfsect) postEndFCheck(symbol);
    else 
    {
	/* The parser has broken - does not recognise which section it is in */ 
	errorvector.push_back(new fatalErr(symbol.line, symbol.col));
    }
    return;
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
    nodevsymflag = 0;
    noconsymflag = 0;
    nomonsymflag = 0;
    noendfsymflag = 0;
    filenotcompleteflag = 1;
    nummonitorsmade = 0; 
}

