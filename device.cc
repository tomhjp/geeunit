#include "device.h"

using namespace std;


/*******************************************************************/
/* Methods for the Device superclass

/* Constructor for the Device class */ 
Device::Device(void)
{
    return;
}

/**********************************************************************/
/* Methods for the devicetype subclasses */ 

/* Checks the number of parameters specified is correct */
bool Device::numParamsValid(int numParams)
{
    if(numParams == validNumParams) return true; 
    else return false;
} 


/* Constructor for the Clk class */ 
Clk::Clk(void)
{
    validNumParams = 1;
}

/* Checks the parameter passed to CLK devices is in the correct range */ 
bool Clk::paramInValidRange(int param) 
{  
    if(param < 65536 && param > 0) return true; 
    else return false; 
}

/* Constructor for the Dtype class */ 
Dtype::Dtype(void)
{
    validNumParams = 0;
}

/* Constructor for the Switch class */ 
Switch::Switch(void)
{
    validNumParams = 1;
}

/* Check that parameter passed to SWITCH devices is valid */ 
bool Switch::paramInValidRange(int param)
{
    if(param==1 || param ==0) return true;
    else return false; 
}

/* Constructor for Gate class */ 
Gate::Gate(void) 
{
    validNumParams = 1;
}

bool Gate::paramInValidRange(int param)
{
    if(param < 17 && param > 0) return true;
    else return false;
}


/* Constructor for Xor class */ 
Xor::Xor(void)
{
    validNumParams = 0; 
}

























