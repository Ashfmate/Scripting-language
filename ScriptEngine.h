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
	enum Type
	{
		Boolean,
		Char,
		Int,
		Double,
		String,
		Count
	};
	using DataType = std::variant<bool, char, int, double, std::string>;
	struct ValueType
	{
		DataType value;
		Type type;
	};
public:
	ScriptEngine()
	{
	}
	ScriptEngine& set_var(std::string name, DataType value);
	ValueType get_var(std::string name);
private:
	
	std::unordered_map<std::string, DataType> variables;
	std::string path;
};