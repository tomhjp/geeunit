#include <string>
using namespace std; 


class Error {
  private:
	static string errorMessage; 
	int lineCalled; 
  public: 
	void printErrMsg(void){
		cout << "Error: " << lineCalled << ": " << errorMessage <<endl;} 		 
	void setLine(int a){
		lineCalled = a; } 
}; 

class InheritedErrorExample : private Error{
	errorMessage = "Particular error message"; 
};


