using namespace std; 

class Device
{
  private: 
    const int validNumParams;    /* Valid number of parameters passed. 1 in all cases in client's original spec */ 
  public:
    Device(void);
    bool numParamsValid(int numParams);  /* Checks the number of parameters passed is correct*/
    bool paramInValidRange(int param);  /* Checks the parameter passed is in a valid range*/ 
};

class Dtype: Device
{ 
  public:
    Dtype(void);
}; 

class Clk: Device
{
  public:  
    Clk(void);
};

class Switch: Device 
{
  public: 
    Switch(void);
};   

class Gate: Device
{
  public: 
    Gate(void);
};

class Xor: Device
{
  public:
    Xor(void);
};
