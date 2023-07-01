#include "ScriptEngine.h"
#include <fmt\format.h>
#include <fmt\ranges.h>

int main()
{
	// TODO
	// *) Make it so that the code struct is actually used where it stores the code
	// *) Make it so that it is possible to set variables
	// *) Add print statment with println
	// *) Add if statment
	// *) Add while and iterate loops
	// *) Add functions
	// *) Add Classes (The hardest of them all)
	// *) Make a var class in code that will allow for any class to be accepted
	ScriptEngine eng("Scripting.txt");
	eng.start();

	system("pause");
	return 0;
}