#include <iostream>

#include "scannertest.h"
#include "scanner.h"

using namespace std;

void printout(symbol_t &s)
{
    cout << "Symbol type: ";
    string str;
    switch (s.symboltype)
    {
        case (strsym)       : str = "strsym";           break;
        case (numsym)       : str = "numsym";           break;
        case (startfsym)    : str = "startfsym";        break;
        case (eofsym)       : str = "eofsym";           break;
        case (badsym)       : str = "badsym";           break;
        case (dotsym)       : str = "dotsym";           break;
        default             : str = "Do more cases";    break;
    }
    cout << str << endl;
    if (s.symboltype == numsym)
        cout << "Number: " << s.num << endl;
    else
        cout << "String: " << s.namestring << endl;
    cout << "Line: " << s.line << endl << "Col: " << s.col << endl << endl;
}

int main(void)
{
    const char* file = "dfile";
    scanner_t scanner(file);

    symbol_t symbol;
    
    /* Read through whole file outputting one symbol and its type at a time */
    while (symbol.symboltype != eofsym)
    {
        scanner.nextSymbol(symbol);
        printout(symbol);
    }
}
