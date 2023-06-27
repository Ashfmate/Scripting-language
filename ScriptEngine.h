#pragma region Preprocessor

#pragma once
#include <variant>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>

#pragma endregion
class ScriptEngine
{
public:
	using DataType = std::variant<bool, char, int, double, std::string>;
	ScriptEngine()
	{
	}
	ScriptEngine& set_var(std::string name, DataType value)
	{
		variables[name] = value;
	}
	DataType get_var(std::string name)
	{
		if (auto it = variables.find(name); it != variables.end())
			return it->second;
		throw script_error("var not found", 0);
	}
private:
	std::unordered_map<std::string, DataType> variables;
	std::string path;
};

class script_error : public std::runtime_error
{
public:
	script_error() = default;
	script_error(std::string msg) : std::runtime_error(msg) {}
	script_error(std::string msg, int err_code) : std::runtime_error(msg), err_code(err_code) {}
	const int which() const
	{
		return err_code;
	}
private:
	int err_code;
};