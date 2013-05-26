#include <iostream>

#include "namestest.h"
#include "names.h"

using namespace std;

int main(void)
{
    names namesObj;
    namesObj.cvtname("Hello");
    namesObj.writename(0);
}
