#include "ScriptEngine.h"
#include <fmt\format.h>
#include <fmt\ranges.h>

int main()
{
	ScriptEngine eng("Scripting.txt");
	eng.start();

	std::vector<std::string> res;

	if (auto him = eng.get_var_range("var1"); him)
	{
		for (size_t i = 0; i < him.value().size(); ++i)
		{
			auto& cur = him.value()[i];
			switch (eng.get_type(cur))
			{
			case ScriptEngine::Boolean:	res.emplace_back(std::get<bool>(cur.value()) ? "True" : "False");	break;
			case ScriptEngine::Int:		res.emplace_back(std::to_string(std::get<int>(cur.value())));		break;
			case ScriptEngine::Double:	res.emplace_back(std::to_string(std::get<double>(cur.value())));	break;
			case ScriptEngine::String:	res.emplace_back(std::get<std::string>(cur.value()));				break;
			case ScriptEngine::Null:	res.emplace_back("NULL");											break;
			}
		}
		fmt::print("The values of \"var1\" are {}\n", res);
	}
	else
	{
		fmt::print("{} : {}\n", him.error().what(), him.error().which());
	}
	res.clear();
	if (auto him = eng.get_var_range("var2"); him)
	{
		for (size_t i = 0; i < him.value().size(); ++i)
		{
			auto& cur = him.value()[i];
			switch (eng.get_type(cur))
			{
			case ScriptEngine::Boolean:	res.emplace_back(std::get<bool>(cur.value()) ? "True" : "False");	break;
			case ScriptEngine::Int:		res.emplace_back(std::to_string(std::get<int>(cur.value())));		break;
			case ScriptEngine::Double:	res.emplace_back(std::to_string(std::get<double>(cur.value())));	break;
			case ScriptEngine::String:	res.emplace_back(std::get<std::string>(cur.value()));				break;
			case ScriptEngine::Null:	res.emplace_back("NULL");											break;
			}
		}
		fmt::print("The values of \"var2\" are {}\n", res);
	}
	else
	{
		fmt::print("{} : {}\n", him.error().what(), him.error().which());
	}
	res.clear();
	if (auto him = eng.get_var_range("Ahmed"); him)
	{
		for (size_t i = 0; i < him.value().size(); ++i)
		{
			auto& cur = him.value()[i];
			switch (eng.get_type(cur))
			{
			case ScriptEngine::Boolean:	res.emplace_back(std::get<bool>(cur.value()) ? "True" : "False");	break;
			case ScriptEngine::Int:		res.emplace_back(std::to_string(std::get<int>(cur.value())));		break;
			case ScriptEngine::Double:	res.emplace_back(std::to_string(std::get<double>(cur.value())));	break;
			case ScriptEngine::String:	res.emplace_back(std::get<std::string>(cur.value()));				break;
			case ScriptEngine::Null:	res.emplace_back("NULL");											break;
			}
		}
		fmt::print("The values of \"Ahmed\" are {}\n", res);
	}
	else
	{
		fmt::print("{} : {}\n", him.error().what(), him.error().which());
	}
	res.clear();
	if (auto him = eng.get_var_range("Ayham"); him)
	{
		for (size_t i = 0; i < him.value().size(); ++i)
		{
			auto& cur = him.value()[i];
			switch (eng.get_type(cur))
			{
			case ScriptEngine::Boolean:	res.emplace_back(std::get<bool>(cur.value()) ? "True" : "False");	break;
			case ScriptEngine::Int:		res.emplace_back(std::to_string(std::get<int>(cur.value())));		break;
			case ScriptEngine::Double:	res.emplace_back(std::to_string(std::get<double>(cur.value())));	break;
			case ScriptEngine::String:	res.emplace_back(std::get<std::string>(cur.value()));				break;
			case ScriptEngine::Null:	res.emplace_back("NULL");											break;
			}
		}
		fmt::print("The values of \"Ayham\" are {}\n", res);
	}
	else
	{
		fmt::print("{} : {}\n", him.error().what(), him.error().which());
	}
	res.clear();
	system("pause");
	return 0;
}