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

name_t names::lookup (symbol_t &symbol)
{
    name_t index;
    for (index=0; index<table.size(); index++)
    {
        if (table[index].namestring == symbol.namestring)
            break;
    }
    if (index == table.size())
    {
        /* str didn't exist, make a namestruct and insert into table */
        namestruct_t namestruct;
        namestruct.namestring = symbol.namestring;
        namestruct.line = symbol.line;
        namestruct.col = symbol.col;

        table.push_back(namestruct);
        return (name_t) table.size()-1;
    }
    else
        return index;
}

name_t names::cvtname (namestring_t str)
{
	name_t index; 
	
    for (index=0; index<table.size(); index++)
    {
        if (table[index].namestring == str)
            break;
    }
    if (index == table.size())
	{
		/* str doesn't yet exist  */ 
		return blankname; 
	}
	else
	{
		/* str exists, and is at location index in the table  */ 
		return index; 
	}	

}

int names::getLine(namestring_t str)
{
	int linenum;
	name_t index; 
	index = cvtname(str); 
	linenum = table[index].line; 
	return linenum;
}

int names::getCol(namestring_t str)
{
	int colnum;
	name_t index; 
	index = cvtname(str); 
	colnum = table[index].col; 
	return colnum;
}


void names::writename (name_t index)
{
    try
    {
        if (index > table.size()-1)
        throw index;
        cout << table[index].namestring;
    }
    catch (name_t index)
    {
        cout << "Warning: Attempted to access index " << index << " outside table size " << \
                table.size() << endl;
    }
}

int names::namelength (name_t index)
{
    return table[index].namestring.length();
}
