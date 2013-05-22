#include <iostream>

#include "scannertest.h"
#include "scanner.h"

using namespace std;

int main(void)
{
    names namesObj;
    const char* file = "testfile";
    scanner_t scanner(&namesObj, file);
    symbol_t symbol; namestring_t name; int num;
    int line, col; bool ok;
    
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.getPosition(line, col, ok);
    cout << line << " " << col << " " << ok << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << num << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.getPosition(line, col, ok);
    cout << line << " " << col << " " << ok << endl;
}
