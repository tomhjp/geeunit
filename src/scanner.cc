#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

#include "scanner.h"

/* Public methods of scanner_t ******************************/
scanner_t::scanner_t(names *namesObjin, const char *defname)
{
    namesObj = namesObjin;
    inf.open(defname);
    if (!inf)
    {
        cout << "Error: cannot open file " << defname << "for reading" << endl;
        exit(1);
    }
}

scanner_t::~scanner_t()
{
    return;
}

void scanner_t::nextSymbol(symbol_t &symbol, namestring_t &namestring, int &num)
{
    eofile = (inf.get(ch) == 0);    // get first character
    if (!eofile)
    {
        if (isspace(ch))
        { // Skip spaces first
            skipspaces();
        }
        if (isdigit(ch))
        { // Then check if the next symbol is a number
            getnumber(num);
            cout << num << endl;     // debugging output
            symbol = numsym;
            return;
        }
        else if (isalpha(ch))
        { // Finally process symbols that are some sort of string
            getname(namestring);
            cout << namestring << endl;       // debugging output
            symbol = symbolType(namestring);
            return;
        }
    }
    else
    {
        symbol = eofsym;
        return;
    }
}

void scanner_t::getPosition(int &oLine, int &oCol, bool &ok)
{
    oLine = line;
    oCol = col;
    ok = true;
}


/* Private methods of scanner_t ******************************/
void scanner_t::skipspaces(void)
{
    while (!eofile)
    {
        if (!isspace(ch))
            break;
        eofile = (inf.get(ch) == 0);
    }
}


void scanner_t::getnumber(int &number)
{
    string num = "";
    while (!eofile)
    {
        if (isdigit(ch))
        {
            num += ch;
        }
        else
        {
            number = atoi(num.c_str());
            break;
        }
        eofile = (inf.get(ch) == 0);
    }
}


name_t scanner_t::getname(namestring_t &str)
{
    namestring_t outstr = "";
    string toolongstr = "";
    bool error = false;

    while (!eofile)
    {
        if (isalnum(ch))
        {
            if (outstr.length() == maxlength)
            {
                error = true;
                toolongstr += ch;
            }
            else
            {
                outstr += ch;
                toolongstr += ch;
            }
        }
        else
        {
            str = outstr;
            if (error)
                cout << "Warning: " << toolongstr << " exceeded maxlength " << maxlength << endl;
            return namesObj->lookup(outstr);
        }
        eofile = (inf.get(ch) == 0);
    }
    return namesObj->lookup(outstr);
}

/* Equate special strings in the definition file to a symbol_t */
symbol_t scanner_t::symbolType(namestring_t namestring)
{
    symbol_t s;
    if      (namestring.compare("STARTFILE"))   s = startfsym;
    else if (namestring.compare("DEVICES"))     s = devsym;
    else if (namestring.compare("CONNECTIONS")) s = connsym;
    else if (namestring.compare("MONITORS"))    s = monsym;
    else if (namestring.compare("END"))         s = endsym;
    else if (namestring.compare("ENDFILE"))     s = endfsym;
    else if (namestring.compare("SWITCH"))      s = switchsym;
    else if (namestring.compare("AND"))         s = andsym;
    else if (namestring.compare("NAND"))        s = nandsym;
    else if (namestring.compare("OR"))          s = orsym;
    else if (namestring.compare("NOR"))         s = norsym;
    else if (namestring.compare("DTYPE"))       s = dtypesym;
    else if (namestring.compare("XOR"))         s = xorsym;
    else if (namestring.compare(","))           s = commasym;
    else if (namestring.compare(";"))           s = semicolsym;
    else if (namestring.compare("("))           s = opsym;
    else if (namestring.compare(")"))           s = cpsym;
    else if (namestring.compare("="))           s = equalsym;
    else if (namestring.compare("."))           s = dotsym;
    else                                        s = strsym;

    return s;
}
