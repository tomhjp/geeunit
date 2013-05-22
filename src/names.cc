#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>

#include "names.h"

using namespace std;


/* Name storage and retrieval routines */

//names::names(void)  /* the constructor */
//{
//    return;
//}

name_t names::lookup (namestring_t str)
{
    name_t index = find(table.begin(), table.end(), str) - table.begin();
    if (index == table.end() - table.begin())
    {
        // str didn't exist, insert str into table
        table.push_back(str);
        return (name_t) table.size()-1;
    }
    else
        return index;
}

name_t names::cvtname (namestring_t str)
{
    return 0;
}

void names::writename (name_t index)
{
    try
    {
        if (index > table.size()-1)
        throw index;
        cout << table[index];
    }
    catch (name_t index)
    {
        cout << "Warning: Attempted to access index " << index << " outside table size " << \
                table.size() << endl;
    }
}

int names::namelength (name_t index)
{
    return table[index].length();
}
