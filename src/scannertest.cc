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
}

int main(void)
{
    /*names namesObj;
    const char* file = "testfile";
    scanner_t scanner(&namesObj, file);

    symbol_t symbol; namestring_t name; int num;*/
    
    /* Read through whole file outputting one symbol and its type at a time */
    /*while (symbol != eofsym)
    {
        scanner.nextSymbol(symbol, name, num);
        printout(symbol);
        if (symbol == numsym)
            cout << num << endl;
        else
            cout << name << endl;
    }*/
}
