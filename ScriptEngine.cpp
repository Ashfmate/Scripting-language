#include "ScriptEngine.h"

ScriptEngine& ScriptEngine::set_var(std::string name, DataType value)
{
	variables[name] = value;
	return *this;
}

ScriptEngine::DataType ScriptEngine::get_var(std::string name)
{
	if (auto it = variables.find(name); it != variables.end())
		return it->second;
	throw std::runtime_error("Variable does not exist");
}

ScriptEngine::Type ScriptEngine::get_type(DataType val)
{
	return Type(val.index());
}
