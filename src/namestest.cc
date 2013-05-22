#include <iostream>

#include "namestest.h"
#include "names.h"

using namespace std;

int main(void)
{
    names* namesObj = new names();
    namesObj->lookup("Hello");
    namesObj->writename(0);
}
