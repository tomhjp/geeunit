#ifndef names_h
#define names_h

#include <string>
#include <vector>

#include "typedefs.h"

using namespace std;

struct namestruct_t {
    namestring_t namestring;
    int line;
    int col;
};

const int maxnames  = 200;  /* max number of distinct names */
const int maxlength = 12;   /* max chars in a name string   */
const int blankname = -1;   /* special name                 */

class names{
    vector<namestruct_t> table;
public:
    name_t lookup (namestring_t str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not already in the name table, it is          */
    /* automatically inserted.                                             */

    name_t cvtname (namestring_t str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not in the name table then 'blankname' is     */
    /* returned.                                                           */

    int getLine (namestring_t str);
    /* Returns the line at which the device with name str was first        */
    /* declared.  used for error reporting.                                */

    int getCol (namestring_t str);
    /* returns the column at which the device with name str was first      */
    /* declared.  used for error reporting                                 */

    void setPos(namestring_t str, int line, int col);

    void writename (name_t index);
    /* Prints out the given name on the console                            */

    int namelength (name_t index);
    /* Returns length ie number of characters in given name                */

    bool indexOk(name_t index);

    names (void);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* names_h */
