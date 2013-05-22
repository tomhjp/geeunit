#ifndef scanner_h
#define scanner_h

class scanner_t
{
    unsigned int line, col;
    char *file;
    names *namesObj;
public:
    scanner_t(names *namesObj, const char *defname);
      /* Constructor for the scanner class
       * Arg1: pointer to an instance of names class
       * Arg2: pointer to the definition file */
    ~scanner_t();
      /* Destructor for the scanner class */
    void nextSymbol(symbol_t &symbol, name_t &name, int &num);
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
