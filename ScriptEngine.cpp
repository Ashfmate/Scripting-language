#include "ScriptEngine.h"

inline ScriptEngine& ScriptEngine::set_var(std::string name, DataType value)
{
	variables[name] = value;
	return *this;
}

inline ScriptEngine::ValueType ScriptEngine::get_var(std::string name)
{
	if (auto it = variables.find(name); it != variables.end())
		return ValueType{it->second, Type(it->second.index())};
	throw std::runtime_error("Variable does not exist");
}
