#pragma region Preprocessor

#pragma once
#include <variant>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>

#pragma endregion

class script_error : public std::runtime_error
{
public:
	script_error() = default;
	script_error(std::string msg) : std::runtime_error(msg), err_code(0){}
	script_error(std::string msg, int err_code) : std::runtime_error(msg), err_code(err_code){}
	const int which() const
	{
		return err_code;
	}
private:
	int err_code;
};

enum Errors : int
{
	Var_Not_Exist,
	Data_Type_Not_Exist
};

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
public:
	ScriptEngine()
	{
	}
	ScriptEngine& set_var(std::string name, DataType value);
	DataType get_var(std::string name, size_t index = 0);
	std::vector<DataType> get_var_range(std::string name);
	Type get_type(DataType val);
private:
	
	std::unordered_map<std::string, std::vector<DataType>> variables;
	std::string path;
private:
};