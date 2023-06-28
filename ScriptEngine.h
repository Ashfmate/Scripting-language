#pragma region Preprocessor

#pragma once
#include <variant>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <vector>
#include <optional>
#include <expected>

#pragma endregion


class script_error
{
public:
	enum Errors : int
	{
		Var_Not_Exist,
		Out_Of_Range
	};
public:
	script_error() = default;
	script_error(std::string msg, Errors err_code) : err_msg(msg), err_code(err_code) { }
	const int which() const
	{
		return err_code;
	}
	const std::string what() const
	{
		return err_msg;
	}
private:
	Errors err_code;
	std::string err_msg;
};


class ScriptEngine
{
public:
	enum Type
	{
		Boolean,
		Int,
		Double,
		String,
		Null,
		Count
	};
	using DataType = std::optional<std::variant<bool, int, double, std::string>>;
public:
	ScriptEngine()
	{
	}
	std::expected<ScriptEngine*, script_error> set_var(std::string name, DataType value, size_t index = 0);
	ScriptEngine& app_var(std::string name, DataType value);
	std::expected<DataType, script_error> get_var(std::string name, size_t index = 0);
	std::expected<std::vector<DataType>, script_error> get_var_range(std::string name);
	Type get_type(DataType val);
private:
	std::unordered_map<std::string, std::vector<DataType>> variables;
	std::string path;
};