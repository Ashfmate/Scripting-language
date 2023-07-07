#include <fmt\core.h>
import Utils;
import Error;
import Parser;
import ScriptEngine;

int main()
{
	// TODO
	// *) Add if statment
	// *) Add while and iterate loops
	// *) Add functions
	// *) Add Classes (The hardest of them all)
	// *) Make a var class in code that will allow for any class to be accepted

	ScriptEngine eng("Scripting.txt");
	if (auto error = eng.runCode(); error != err::ErrorCode::No_Error)
		fmt::print("Error code : {} FIX IT BOI", (int)error);
	else
		fmt::print("\n\n\n\n Great, the code ran perfectly");
	system("pause");
	return 0;
}