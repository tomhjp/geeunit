#ifndef device_h
#define	device_h

using namespace std; 

class Device
{
  protected: 
    int validNumParams;   /* Valid number of parameters passed. 1 in all cases in client's original spec */ 
  public:
    Device(void);
    bool numParamsValid(int numParams);  /* Checks the number of parameters passed is correct*/
    bool paramInValidRange(int param);  /* Checks the parameter passed is in a valid range, as specified in child classes */ 
};

class Dtype: public Device
{ 
  public:
    Dtype(void);
}; 

class Clk: public Device
{
  public:  
    Clk(void);
    bool paramInValidRange(int param);
};

class Switch: public Device 
{
  public: 
    Switch(void);
    bool paramInValidRange(int param);
};   

class Siggen: public Device
{
  public:
    Siggen(void);
    bool param1InValidRange(int param); 
    bool paramNInValidRange(int param);
};

class Gate: public Device
{
  public: 
    Gate(void);
    bool paramInValidRange(int param);
};

class Xor: public Device
{
  public:
    Xor(void);
};

#endif
