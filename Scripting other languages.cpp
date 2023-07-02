#include "ScriptEngine.h"
#include <fmt\core.h>

void print(ScriptEngine& eng,const std::string& name)
{
	if (auto& cur = eng.get_var_range(name); cur)
	{
		std::string out;
		fmt::print("The values of {} are ", name);
		bool given_val = false;
		for (auto& elem : cur.value())
		{
			given_val = true;
			switch (eng.get_type(elem))
			{
			case ScriptEngine::Boolean:
				out += (std::get<bool>(elem.value()) ? " True ," : " False ,");
				break;
			case ScriptEngine::Int:
				out += " " + std::to_string(std::get<int>(elem.value())) + " ,";
				break;
			case ScriptEngine::Double:
				out += " " + std::to_string(std::get<double>(elem.value())) + " ,";
				break;
			case ScriptEngine::String:
				out += " \"" + std::get<std::string>(elem.value()) + "\" ,";
				break;
			case ScriptEngine::Null:
				out += " Null ,";
				break;
			default:
				break;
			}
		}
		if (given_val)
		{
			out.pop_back();
			out.back() = ']';
			out.front() = '[';
		}
		fmt::print("{}\n", out);
	}
	else
		fmt::print("{} : {}\n", cur.error().what(), cur.error().which());
}

int main()
{
	// TODO
	// *) Make it so that it is possible to set variables
	// *) Add print statment with println
	// *) Add if statment
	// *) Add while and iterate loops
	// *) Add functions
	// *) Add Classes (The hardest of them all)
	// *) Make a var class in code that will allow for any class to be accepted
	ScriptEngine eng("Scripting.txt");
	eng.start();

	print(eng, "var1");
	print(eng, "var2");
	print(eng, "Ahmed");
	print(eng, "Ayham");

	system("pause");
	return 0;
}