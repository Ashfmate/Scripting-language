#include "ScriptEngine.h"
#include <fmt\format.h>
#include <fmt\ranges.h>

int main()
{
	ScriptEngine eng;

	eng.set_var("double", 5.23423);
	eng.set_var("double", "Hello boi");
	eng.set_var("double", 1991);
	eng.set_var("double", std::nullopt);

	std::vector<std::string> res;

	if (auto him = eng.get_var_range("double"); him)
	{
		for (size_t i = 0; i < him.value().size(); ++i)
		{
			auto& cur = him.value()[i];
			switch (eng.get_type(cur))
			{
			case ScriptEngine::Boolean:	res.emplace_back(std::get<bool>(cur.value()) ? "True" : "False");	break;
			case ScriptEngine::Char:	res.emplace_back(std::string(std::get<char>(cur.value()), 1));		break;
			case ScriptEngine::Int:		res.emplace_back(std::to_string(std::get<int>(cur.value())));		break;
			case ScriptEngine::Double:	res.emplace_back(std::to_string(std::get<double>(cur.value())));	break;
			case ScriptEngine::String:	res.emplace_back(std::get<std::string>(cur.value()));				break;
			case ScriptEngine::Null:	res.emplace_back("NULL");											break;
			}
		}
		fmt::print("The values of \"double\" are {}\n", res);
	}
	else
	{
		fmt::print("{} : {}\n", him.error().what(), him.error().which());
	}

	system("pause");
	return 0;
}