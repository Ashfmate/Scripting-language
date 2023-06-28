#include "ScriptEngine.h"

std::expected<ScriptEngine*, script_error> ScriptEngine::set_var(std::string name, DataType value, size_t index)
{
	if (auto& it = variables[name]; index == 0)
		it.emplace_back(value);
	else if (index > it.size())
		return std::unexpected(script_error("Index out of range", script_error::Out_Of_Range));
	else
		it[index] = value;
	return this;
}

ScriptEngine& ScriptEngine::app_var(std::string name, DataType value)
{
	variables[name].push_back(value);
	return *this;
}

std::expected<ScriptEngine::DataType, script_error> ScriptEngine::get_var(std::string name, size_t index)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected(script_error("Variable does not exist", script_error::Var_Not_Exist));
	else
		return variables[name][index];
}

std::expected<std::vector<ScriptEngine::DataType>, script_error> ScriptEngine::get_var_range(std::string name)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected{script_error("Variable does not exist", script_error::Var_Not_Exist)};
	else
		return variables[name];
}

ScriptEngine::Type ScriptEngine::get_type(DataType val)
{
	if (val)
		return Type(val.value().index());
	return Null;
}
