#include <string>

using namespace std; 


class Error 
{
	int lineCalled; 
  public: 
    string errorMessage; 
	void printErrMsg(void); 
	void setLine(int a);
    Error(void);
}; 

class Example : Error
{
    public:
    Example(void);
};


