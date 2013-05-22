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
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << num << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
    scanner.nextSymbol(symbol, name, num);
    cout << name << endl;
}
