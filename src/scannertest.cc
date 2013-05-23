#include <iostream>

#include "scannertest.h"
#include "scanner.h"

using namespace std;

void printout(symbol_t s)
{
    cout << "Symbol type: ";
    string str = "";
    switch (s)
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
    names namesObj;
    const char* file = "testfile";
    scanner_t scanner(&namesObj, file);
    symbol_t symbol = strsym; namestring_t name; int num;
    int line, col; bool ok;
    
    int i = 1;
    while (symbol != eofsym)
    {
        //cout << "Symbol: " << i++ << endl;
        scanner.nextSymbol(symbol, name, num);
        /*if (symbol == numsym)
            cout << num << endl;
        else
            cout << name << endl;*/
        printout(symbol);
        cout << name << " " << num << endl;
    }
    /*
    scanner.nextSymbol(symbol, name, num);
    cout << name << " " << num << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << " " << num << endl;
    //scanner.getPosition(line, col, ok);
    //cout << line << " " << col << " " << ok << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << " " << num << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << " " << num << endl;
    //scanner.getPosition(line, col, ok);
    //cout << line << " " << col << " " << ok << endl;*/
}
