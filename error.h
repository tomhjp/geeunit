#include <string>

using namespace std; 


class Error 
{
  protected:  
	int lineCalled; 
	string errorMessage; 
  public: 
	void printErrMsg(void); 
	void setLine(int a);
    Error(void);
}; 

class Example : Error
{
    public:
    Example(void);
};


