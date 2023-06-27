#include "ScriptEngine.h"

ScriptEngine& ScriptEngine::set_var(std::string name, DataType value)
{
	variables[name].push_back(value);
	return *this;
}

ScriptEngine::DataType ScriptEngine::get_var(std::string name, size_t index)
{
	if (auto it = variables.find(name); it == variables.end())
		throw script_error("Variable does not exist", Var_Not_Exist);
	else
		return variables[name][index];
}

std::vector<ScriptEngine::DataType> ScriptEngine::get_var_range(std::string name)
{
	if (auto it = variables.find(name); it == variables.end())
		throw script_error("Variable does not exist", Var_Not_Exist);
	else
		return variables[name];
}

ScriptEngine::Type ScriptEngine::get_type(DataType val)
{
	return Type(val.index());
}
