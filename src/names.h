#ifndef names_h
#define names_h

#include <string>
#include <vector>

using namespace std;

const int maxnames  = 200;  /* max number of distinct names */
const int maxlength = 12;   /* max chars in a name string   */
const int blankname = -1;   /* special name                 */

typedef unsigned int name_t;
typedef string namestring_t;

class names{
    vector<namestring_t> table;
public:
    name_t lookup (namestring_t str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not already in the name table, it is          */
    /* automatically inserted.                                             */

    name_t cvtname (namestring_t str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not in the name table then 'blankname' is     */
    /* returned.                                                           */

    void writename (name_t index);
    /* Prints out the given name on the console                            */

    int namelength (name_t index);
    /* Returns length ie number of characters in given name                */

    //names (void);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* names_h */



