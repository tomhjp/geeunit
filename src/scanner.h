#ifndef scanner_h
#define scanner_h

#include <fstream>
#include <string>

#include "typedefs.h"
#include "names.h"

typedef enum {validPunc, invalidPunc, slash, star} punc_t;

/********************************************************/
/* The scanner class reads in a file and populates
 * symbol_t structs with information about the next
 * relevant string in the definition file. It does this
 * for one definition file string every time nextSymbol
 * is called
 ********************************************************/

class scanner_t
{
private:
    ifstream inf;           // input file object
    const char *file;       // input file string
    char ch;                // current character
    bool eofile;            // true when end of file is reached
    unsigned int line, col; // current line and column numbers
    
    void skipspaces(void);
      /* Skip spaces; will usually end on \n at eof */

    void getnumber(symbol_t &symbol);
      /* Process a string that starts with a digit */

    void getname(symbol_t &symbol);
      /* Process a string that starts with an alpha character */

    void getpunc(symbol_t &symbol);
      /* Process a string that doesn't start with space, alpha or digit */

    symboltype_t symbolType(namestring_t namestring);
      /* Return the symbol_t for a particular string such as 'END' or '.' */

    void incrementPosition(void);
      /* Track line and column number */

    punc_t puncType(char ch);
      /* Return the type of punctuation, and invalidPunc if the
       * punctuation shouldn't appear in a definition file */

    bool skipcomment(void);
      /* Process a string (hopefully a comment) that starts with a '/'
       * character */
    
    void saveCurPosition(symbol_t &symbol);
      /* Save the current line and col values into symbol and subtract
       * 1 from col to make it correct */

    void replaceTabsWithSpaces(const char *defname);

    void saveScannerState(streampos &pos, char &chstart, unsigned int &linestart, unsigned int &colstart, bool &eofilestart);

    void restoreScannerState(streampos pos, char chstart, unsigned int linestart, unsigned int colstart, bool eofilestart);

public:
    scanner_t(string defname);
      /* Constructor for the scanner class
       * Arg1: pointer to the definition file */

    ~scanner_t();
      /* Destructor for the scanner class */

    void nextSymbol(symbol_t &symbol);
      /* Return the next symbol in the definition file in a struct */

    void closeDefinitionFile(void);

    void printError(int line, int col, string errorStr);
};

#endif  /* scanner_h */
