#include "ScriptEngine.h"
#include <fmt\core.h>


int main()
{
	// TODO
	// *) Add if statment
	// *) Add while and iterate loops
	// *) Add functions
	// *) Add Classes (The hardest of them all)
	// *) Make a var class in code that will allow for any class to be accepted
	ScriptEngine eng;
	auto res = eng.start("Scripting.txt");
	eng.exec();
	fmt::print("\n\n\n\n{} : Error Code [{}]\n", res.what(), res.which());
	system("pause");
	return 0;
}