
#include <iostream>
#include <vector>
 
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "typedefs.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"


using namespace std;

int main(void)
{
    names *nmz;    // pointer to the names class
    network *netz; // pointer to the network class
    devices *dmz;  // pointer to the devices class
    monitor *mmz;  // pointer to the monitor class
    scanner_t *scz; 
    
    vector<Error*> errorvector;
    
    const char* file = "dfile";
    nmz = new names();
    netz = new network(nmz);
    dmz = new devices(nmz, netz);
    mmz = new monitor(nmz, netz);
    scz = new scanner_t(file);

    symbol_t symbol;
    parser myparser(netz, dmz, mmz, scz, nmz);
    
    /* Read through whole file and parse it */
    while (symbol.symboltype != eofsym)
    {
        scz->nextSymbol(symbol);
	//cout << symbol.namestring << endl; 
        myparser.readin(symbol);
	
    }
    
    errorvector = myparser.getErrorVector();
    for(int i=0; i<errorvector.size(); i++)
	errorvector[i]->printErrMsg();
	
}
