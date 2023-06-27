#include "ScriptEngine.h"
#include <conio.h>
#include <fmt\format.h>

int main()
{
	/*Engine eng;

	eng.run_engine("Scripting.txt");*/

	ScriptEngine eng;

	eng.set_var("double", double(5.0));

	switch (eng.get_type(eng.get_var("double")))
	{
	case ScriptEngine::Boolean:
		fmt::print("This some good bool");
		break;
	case ScriptEngine::Char:
		fmt::print("This some good char");
		break;
	case ScriptEngine::Double:
		fmt::print("This some good double");
		break;
	case ScriptEngine::Int:
		fmt::print("This some good int");
		break;
	case ScriptEngine::String:
		fmt::print("This some good string");
		break;
	default:
		fmt::print("This some good Nothing");
		break;
	}

	while (!_kbhit());
	return 0;
}