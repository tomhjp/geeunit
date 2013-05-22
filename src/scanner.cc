#include <iostream>
#include <string>
#include <cstdlib>

#include "scanner.h"

scanner_t::scanner_t(names *namesObj, const char *defname)
{
    namesObj = namesObj;
    file = defname;
}

scanner_t::~scanner_t()
{
    
}

scanner_t::void nextSymbol(symbol_t &symbol, name_t &name, int &num)
{
    
}

scanner_t::void getPosition(int &line, int &col, bool &ok)
{
    
}
