#ifndef scanner_h
#define scanner_h

#include <fstream>
#include <string>

#include "names.h"

typedef enum {strsym, numsym, startfsym, devsym, connsym, monsym, endsym, endfsym,
              switchsym, andsym, nandsym, orsym, norsym, dtypesym, xorsym,
              commasym, semicolsym, opsym, cpsym, equalsym, dotsym,
              badsym, eofsym} symbol_t;
              /* opsym and cpsym = open and close parenthesis sym */

typedef enum {validPunc, invalidPunc, slash, star} punc_t;

class scanner_t
{
private:
    ifstream inf;           // input file
    names *namesObj;        // names object
    char ch;                // current character
    bool eofile;            // true when end of file is reached
    unsigned int line, col; // current line and column numbers
    
    void skipspaces(void);
      /* Skip spaces; will usually end on \n at eof */

    void getnumber(int &number);
      /* Process a string that starts with a digit */

    name_t getname(namestring_t &str);
      /* Process a string that starts with an alpha character */

    void getpunc(namestring_t &str);
      /* Process a string that doesn't start with space, alpha or digit */

    symbol_t symbolType(namestring_t namestring);
      /* Return the symbol_t for a particular string such as 'END' or '.' */

    void incrementPosition(void);
      /* Track line and column number */

    punc_t puncType(char ch);
      /* Return the type of punctuation, and invalidPunc if the
       * punctuation shouldn't appear in a definition file */

    bool skipcomment(void);
      /* Process a string (hopefully a comment) that starts with a '/'
       * character */

public:
    scanner_t(names *namesObjin, const char *defname);
      /* Constructor for the scanner class
       * Arg1: pointer to an instance of names class
       * Arg2: pointer to the definition file */

    ~scanner_t();
      /* Destructor for the scanner class */

    void nextSymbol(symbol_t &symbol, namestring_t &namestring, int &num);
      /* Return the next symbol in the definition file 
       * Arg1: Type of symbol, e.g. semicol, namesym numsym etc.
       * Arg2: Returns the name if the symbol has one
       * Arg3: Returns the number if the symbol is a number */

    void getPosition(int &line, int &col, bool &ok);
      /* Return the position of the last symbol the scanner scanned 
       * Arg1: line number
       * Arg2: column number
       * Arg3: flag to check function executed correctly */
};

#endif  /* scanner_h */
