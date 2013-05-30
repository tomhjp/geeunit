#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
//#include <boost>

#include "scanner.h"

/***********************************************************/
/************** Private methods of scanner_t ***************/
/***********************************************************/
void scanner_t::skipspaces(void)
{
    while (!eofile)
    {
        if (!isspace(ch) && (int)ch != 10)
            break;
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
}


/* Called when '/' is found */
bool scanner_t::skipcomment(void)
{
    /* Save state of scanner in case we're not actually processing a
     * comment and need to backtrack */
    char chstart; unsigned int linestart, colstart;
    bool eofilestart; streampos pos;
    saveScannerState(pos, chstart, linestart, colstart, eofilestart);

    /* Setup for searching for the comment */
    string str = "";
    bool prefixFound = false;
    bool suffixFound = false;
    string prefix("/*");
    string suffix("*/");
    
    /* Start searching */
    while (!eofile && (!prefixFound || !suffixFound))
    {
        str += ch;          // save comment so we can search for key characters
        if (!prefixFound)
        {
            /* Look for prefix */
            if (str.compare(0,prefix.size(),prefix) == 0)
                prefixFound = true;
        }
        if (!prefixFound && str.size() >= 2)
            break;          // break if the string didn't start with '/*' afterall
        if (prefixFound && !suffixFound)
        {
            /* Look for suffix */
            if (str.compare(str.size()-2,str.size(),suffix) == 0)
                suffixFound = true;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
    /* No comment found */
    if (!prefixFound)
    {
        /* Return scanner to state before skipcomment was called */
        restoreScannerState(pos, chstart, linestart, colstart, eofilestart);
        return false;
    }
    /* Comment found, return true */
    else
        return true;        // prefixFound && !suffixFound => whole file is comment after prefix
}


void scanner_t::getnumber(symbol_t &symbol)
{
    string strnum = "";         // string to build up num in
    while (!eofile)
    {
        if (isdigit(ch))
            strnum += ch;       // build up the number as a string
        else
        {
            symbol.num = atoi(strnum.c_str());     // convert string to int at end
            symbol.symboltype = numsym;
            break;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
}


void scanner_t::getname(symbol_t &symbol)
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
                /* Exceeded maxlength */
                error = true;
                toolongstr += ch;
            }
            else
            {
                /* Normal operation before maxlength reached */
                outstr += ch;
                toolongstr += ch;
            }
        }
        else
        {
            /* end of name string when no longer alpha characters */
            symbol.namestring = outstr;
            symbol.symboltype = symbolType(outstr);
            if (error)
                cout << "Warning: " << toolongstr << " exceeded maxlength " << maxlength << endl;
            return;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
    symbol.namestring = outstr;
    symbol.symboltype = symbolType(outstr);
    return;
}
    

void scanner_t::getpunc(symbol_t &symbol)
{
    namestring_t str = "";
    
    /* Only want to retrieve one punctuation symbol at a time, so if rather than while */
    if (!eofile)
    {
        if (ch == '-')
        {
            /* Start looking for '->' so save scanner state to return if not found (badsym) */
            eofile = (inf.get(ch) == 0);
            incrementPosition();
            char chstart; unsigned int linestart, colstart;
            bool eofilestart; streampos pos;
            saveScannerState(pos, chstart, linestart, colstart, eofilestart);
            if (ch == '>')
            {
                symbol.namestring = "->";
                symbol.symboltype = symbolType("->");
                eofile = (inf.get(ch) == 0);
                incrementPosition();
                return;
            }
            else
            {
                symbol.namestring = "-";
                symbol.symboltype = symbolType("-");
                restoreScannerState(pos, chstart, linestart, colstart, eofilestart);
                return;
            }
        }
        /* If is not space, alphabetic or digit */
        if (!isalnum(ch) && !isspace(ch))
        {
            str += ch;
            symbol.symboltype = symbolType(str);
            symbol.namestring = str;
        }
        else
        {
            return;
        }
        eofile = (inf.get(ch) == 0);
        incrementPosition();
    }
}


/* Equate special strings in the definition file to a symbol_t
 * Only called for non-numbers */
symboltype_t scanner_t::symbolType(namestring_t namestring)
{
    symboltype_t s;
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
    else if (!namestring.compare("CLOCK"))       s = clksym;   // to prevent interference with dtype.CLK input
    else if (!namestring.compare(","))           s = commasym;
    else if (!namestring.compare(";"))           s = semicolsym;
    else if (!namestring.compare("("))           s = opsym;
    else if (!namestring.compare(")"))           s = cpsym;
    else if (!namestring.compare("="))           s = equalsym;
    else if (!namestring.compare("."))           s = dotsym;
    else if (!namestring.compare("->"))          s = connpuncsym;
    else if (!namestring.compare("DATA"))        s = ddatasym;
    else if (!namestring.compare("SET"))         s = dsetsym;
    else if (!namestring.compare("CLEAR"))       s = dclearsym; 
    else if (!namestring.compare("CLK"))         s = dclksym;
    else if (!namestring.compare("Q"))           s = qsym;
    else if (!namestring.compare("QBAR"))        s = qbarsym; 
    /* If first char is alphabetic then namestring was retrieved with getname and is a name */
    else if (isalpha(firstchar))                 s = strsym;
    else if (!namestring.compare("\0"))          s = eofsym;
    else                                         s = badsym;

    return s;
}


void scanner_t::incrementPosition(void)
{
    if (!eofile)    // only want to increment position if we haven't hit eofile
    {
        col++;
        if (ch == '\n')
        {
            line++;
            col = 0;
        }
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


void scanner_t::saveCurPosition(symbol_t &symbol)
{
    symbol.line = line;
    symbol.col = col;     // col represents col about to be read in scanner class
}


/* Goes through file and replaces tabs with spaces to facilitate accurate reporting of column numbers */
void scanner_t::replaceTabsWithSpaces(const char *defname)
{
    char ch;
    bool eofile;
    bool tabFound = false;

    ifstream infile;
    infile.open(defname);
    if (!infile)
    {
        cout << "Error: cannot open file " << defname << "for reading" << endl;
        exit(1);
    }
    string fileString = "";
    eofile = (infile.get(ch) == 0);
    while (!eofile)
    {
        if (ch == '\t')
        {
            tabFound = true;
            fileString += "    ";
        }
        else
            fileString += ch;
        eofile = (infile.get(ch) == 0);
    }
    infile.close();
    if (tabFound)
    {
        ofstream outfile;
        outfile.open(defname);
        outfile << fileString;
        outfile.close();
    }
}


void scanner_t::saveScannerState(streampos &pos, char &chstart, unsigned int &linestart, unsigned int &colstart, bool &eofilestart)
{
    chstart = ch;
    linestart = line;
    colstart = col;
    eofilestart = eofile;
    pos = inf.tellg();
}


void scanner_t::restoreScannerState(streampos pos, char chstart, unsigned int linestart, unsigned int colstart, bool eofilestart)
{
    ch = chstart;
    line = linestart;
    col = colstart;
    eofile = eofilestart;
    inf.seekg(pos);
}





/***********************************************************/
/************** Public methods of scanner_t ****************/
/***********************************************************/
scanner_t::scanner_t(string deffile)
{
    file = deffile.c_str();

    replaceTabsWithSpaces(file);

    inf.open(file);
    if (!inf)
    {
        cout << "Error: cannot open file " << file << "for reading" << endl;
        exit(1);
    }

    /*string fileString = fileToString(defname);
    boost::replace_all(fileString, '\t', "    ");
    infile.close();
    ofstream(defname) << fileString;*/

    line = 1;   // first line is line 1
    col = 0;    // First possible cursor position is column 1, but incrementPosition will be called
                // as soon as a character is read in
    
    eofile = (inf.get(ch) == 0);    // get first character
    incrementPosition();
}


scanner_t::~scanner_t()
{
    inf.close();
    return;
}


void scanner_t::nextSymbol(symbol_t &symbol)
{
    if (!eofile)
    {
        /* Skip spaces first */
        skipspaces();

        /* If the character is a slash, guess that it's going to be a comment */
        if (puncType(ch) == slash)
        {
            /* skipcomment discards everything in between comment
             * characters and returns the input file to the place it
             * started if it turns out the '/' character wasn't followed
             * by '*' */
            bool isComment = skipcomment();
            if (!isComment)
            {
                /* Turns out it was some sort of string starting with '/' */
                saveCurPosition(symbol);
                getpunc(symbol);
                return;
            }
            else if (isComment)
            {
                /* Comment has been discarded, now call nextSymbol so
                 * that the variables passed in actually get populated */
                nextSymbol(symbol);
                return;
            }
        }

        /* Now at the beginning of what we will output as the next symbol */
        saveCurPosition(symbol);

        /* Check if the next symbol is a number */
        if (isdigit(ch))
        {
            getnumber(symbol);
            return;
        }

        /* Process symbols that are some sort of string */
        else if (isalpha(ch))
        {
            getname(symbol);
            return;
        }

        else
        {
            /* Some sort of string starting with punctuation */
            getpunc(symbol);
            symbol.symboltype = symbolType(symbol.namestring);
            /* ch will still be \n if eofile is reached so overwrite it
             * if the end of the file has been read in already */
            if (eofile)
            {
                symbol.symboltype = eofsym;
                symbol.line -= 1;
            }
            return;
        }
    }

    else
    {
        symbol.symboltype = eofsym;
        symbol.line -= 1;
        return;
    }
}

void scanner_t::printError(int line, int col, string errorStr, bool hasPosition)
{
    // clear the eofbit so that we can successfully seek the beginning of the file again
    inf.clear();
    inf.seekg(inf.beg);
    
    /* Only try to label a position if it is an error with a position
     * For example, missing inputs do not have their location printed out */
    if (hasPosition)
    {
        string lineStr;
        /* Start looping through lines until we get to the right line */
        for (int l=0; l<line; l++)
        {
            lineStr = "";
            if (!getline(inf, lineStr))
            {
                // failed to read this line from the file (it didn't exist)
                cout << "Bad line value, " << line << ". Seems to be outside the line " << \
                        "range of the file, " << l+1 << endl;
                return;
            }
            else if ((col > lineStr.length()) && (l+1 == line))
            {
                // if col is outside range on the line we are looking for
                cout << "Bad col value " << col << ". Greater than line length " \
                     << lineStr.length() << " of line " << l+1 << endl;
                return;
            }
            else if (l+1 == line)
            {
                // reached the line on which an error is being reported without line or col fault
                cout << lineStr << endl;
                for (int i=0; i<col-1; i++)
                    cout << " ";
                cout << "^" << endl;
                cout << errorStr << endl;
            }
        }
    }
    else
        cout << errorStr << endl;
}
