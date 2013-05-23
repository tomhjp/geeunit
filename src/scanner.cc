#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

#include "scanner.h"

/***********************************************************/
/************** Public methods of scanner_t ****************/
/***********************************************************/
scanner_t::scanner_t(names *namesObjin, const char *defname)
{
    namesObj = namesObjin;
    inf.open(defname);
    if (!inf)
    {
        cout << "Error: cannot open file " << defname << "for reading" << endl;
        exit(1);
    }
    line = 1;   // first line is line 1
    col = 1;    // First possible cursor position is column 1
    
    eofile = (inf.get(ch) == 0);    // get first character
    incrementPosition();
}

scanner_t::~scanner_t()
{
    return;
}

void scanner_t::nextSymbol(symbol_t &symbol, namestring_t &namestring, int &num)
{
    if (!eofile)
    {
        // Skip spaces first
        skipspaces();
        // Then check if the next symbol is a number
        if (isdigit(ch))
        {
            cout << "NUMBER" << endl;
            getnumber(num);
            symbol = numsym;
            return;
        }
        // Finally process symbols that are some sort of string
        else if (isalpha(ch))
        {
            cout << "NAME" << endl;
            getname(namestring);
            symbol = symbolType(namestring);
            return;
        }
        else if (puncType(ch) == slash)
        {
            cout << "COMMENT" << endl;
            // namestring will be unaffected if the following ch is '*'
            // i.e. namestring unaffected if it really is a comment.
            // Otherwise, skipcomment needs to function like getpunc method
            bool isComment = skipcomment();
            if (!isComment)
            {
                cout << "/ WITHOUT COMMENT" << endl;
                getpunc(namestring);
                symbol = symbolType(namestring);
                return;
            }
            else
            {
                // found a comment so now look at next symbol
                cout << "COMMENT - CALLING NEXTSYMBOL" << endl;
                nextSymbol(symbol, namestring, num);
                //cout << "Skipped comment, ch = " << ch << endl;
            }
        }
        else
        {
            cout << "PUNC/MISCELLANEOUS" << endl;
            getpunc(namestring);
            symbol = symbolType(namestring);
            return;
        }
    }
    else
    {
        cout << "EOF" << endl;
        symbol = eofsym;
        return;
    }
}

void scanner_t::getPosition(int &oLine, int &oCol, bool &ok)
{
    ok = true;
    if (line >= 0) oLine = line;
    else ok = false;
    if (col >= 0) oCol = col-1;     // column has moved on 1 since the last symbol outputted
    else ok = false;
}


/***********************************************************/
/************** Private methods of scanner_t ***************/
/***********************************************************/
void scanner_t::skipspaces(void)
{
    while (!eofile)
    {
        if (!isspace(ch))
            break;
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
}

/* Called when '/' is found */
bool scanner_t::skipcomment(void)
{
    //cout << "SKIPCOMMENT" << endl;
    char chstart = ch;
    streampos pos = inf.tellg();

    string str = "";
    bool prefixFound = false;
    bool suffixFound = false;
    string prefix("/*");
    string suffix("*/");
    
    while (!eofile && (!prefixFound || !suffixFound))
    {
        str += ch;
        if (!prefixFound)
        {
            if (str.compare(0,prefix.size(),prefix) == 0)
                prefixFound = true;
        }
        if (!prefixFound && str.size() >= 2)
            break;
        if (prefixFound && !suffixFound)
        {
            if (str.compare(str.size()-2,str.size(),suffix) == 0)
                suffixFound = true;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
    if (!prefixFound)
    {
        return false;       // no comment found
        ch = chstart;       // go back to where skipcomment started
        //inf.seekg(pos);
    }
    else
        return true;        // prefixFound && !suffixFound => whole file is comment after prefix
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
        incrementPosition();
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
        incrementPosition();
    }
    return namesObj->lookup(outstr);
}

void scanner_t::getpunc(namestring_t &str)
{
    str = "";
    
    while (!eofile)
    {
        if (!isalnum(ch) && !isspace(ch))
        {
            str += ch;
        }
        else
        {
            //cout << "getpunc got " << str << endl;
            return;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
}

/* Equate special strings in the definition file to a symbol_t
 * Only called for non-numbers */
symbol_t scanner_t::symbolType(namestring_t namestring)
{
    symbol_t s;
    char firstchar = namestring[0];

    /* namestring.compare(string) returns 0 if namestring and string identical */
    if      (!namestring.compare("STARTFILE"))   s = startfsym;
    else if (!namestring.compare("DEVICES"))     s = devsym;
    else if (!namestring.compare("CONNECTIONS")) s = connsym;
    else if (!namestring.compare("MONITORS"))    s = monsym;
    else if (!namestring.compare("END"))         s = endsym;
    else if (!namestring.compare("ENDFILE"))     s = endfsym;
    else if (!namestring.compare("SWITCH"))      s = switchsym;
    else if (!namestring.compare("AND"))         s = andsym;
    else if (!namestring.compare("NAND"))        s = nandsym;
    else if (!namestring.compare("OR"))          s = orsym;
    else if (!namestring.compare("NOR"))         s = norsym;
    else if (!namestring.compare("DTYPE"))       s = dtypesym;
    else if (!namestring.compare("XOR"))         s = xorsym;
    else if (!namestring.compare(","))           s = commasym;
    else if (!namestring.compare(";"))           s = semicolsym;
    else if (!namestring.compare("("))           s = opsym;
    else if (!namestring.compare(")"))           s = cpsym;
    else if (!namestring.compare("="))           s = equalsym;
    else if (!namestring.compare("."))           s = dotsym;
    /* If first char is alphabetic then namestring was retrieved with getname and is a name */
    else if (isalpha(firstchar))                 s = strsym;
    else                                         s = badsym;

    return s;
}

void scanner_t::incrementPosition(void)
{
    col++;
    if (ch == '\n')
    {
        line++;
        col = 0;
    }
}

punc_t scanner_t::puncType(char ch)
{
    if ((ch == ',') || (ch == ';') || (ch == '(') || (ch == ')') || (ch == '=') || (ch == '.'))
        return validPunc;
    else if (ch == '/')
        return slash;
    else if (ch == '*')
        return star;
    else
        return invalidPunc;
}
