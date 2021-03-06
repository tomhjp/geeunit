#ifndef typedefs_h
#define typedefs_h

#include <string>

using namespace std;

typedef int name_t;
typedef string namestring_t;

typedef enum {strsym, numsym, startfsym, devsym, connsym, monsym, endsym, endfsym,
              switchsym, andsym, nandsym, orsym, norsym, dtypesym, xorsym, clksym,
              siggensym, commasym, semicolsym, opsym, cpsym, equalsym, dotsym, connpuncsym,
              badsym, eofsym, ddatasym, dclksym, dsetsym, dclearsym, qsym, qbarsym} symboltype_t;
              /* opsym and cpsym = open and close parenthesis sym */

struct symbol_t {
    symboltype_t symboltype;
    namestring_t namestring;
    int num;
    unsigned int line;
    unsigned int col;
};

#endif      // typedefs_h
