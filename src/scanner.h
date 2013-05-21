#ifndef scanner_h
#define scanner_h

class scanner_t
{
    unsigned int line, col;
public:
    scanner_t();
    void getPosition(int &line, int &col, bool &ok);
    
};

#endif  /* scanner_h */
