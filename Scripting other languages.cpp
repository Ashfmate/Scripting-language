#include <fmt\core.h>
import Utils;
import Error;
import Parser;

int main()
{
	// TODO
	// *) Add if statment
	// *) Add while and iterate loops
	// *) Add functions
	// *) Add Classes (The hardest of them all)
	// *) Make a var class in code that will allow for any class to be accepted

	auto res = parse::parseCode("Scripting.txt");

	if (res)
		fmt::print("Horray, nothing wrong happened\n");
	else
		fmt::print("Error code :{}\n", (int)res.error());

	system("pause");
	return 0;
}