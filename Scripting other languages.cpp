#include "ScriptEngine.h"
#include <conio.h>
#include <fmt\format.h>

int main()
{
	/*Engine eng;

	eng.run_engine("Scripting.txt");*/

	ScriptEngine eng;

	eng.set_var("double", double(5.0));

	try
	{
		auto var = eng.get_var("h");

	}
	catch (const script_error& e)
	{
		fmt::print("{} : error code {}", e.what(), e.which());
	}

	while (!_kbhit());
	return 0;
}