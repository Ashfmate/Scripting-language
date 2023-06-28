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
#include <algorithm>
#include <memory>

#pragma endregion

std::string to_lower(std::string word);

class script_error
{
public:
	enum Errors : int
	{
		Var_Not_Exist,
		Out_Of_Range,
		Type_Not_Supported
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
using DataType = std::optional<std::variant<bool, int, double, std::string>>;

struct Code
{
	std::optional<bool> expr;
	std::vector<DataType> data_types;
	std::optional<std::string> var_name;
	std::optional<size_t> idx;
	std::optional<std::vector<std::string>> keywords;
	std::unique_ptr<Code> inner_code;
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
public:
	ScriptEngine(std::string path);
	std::expected<ScriptEngine*, script_error> set_var(std::string name, DataType value, size_t index = 0);
	ScriptEngine& app_var(std::string name, DataType value);
	std::expected<DataType, script_error> get_var(std::string name, size_t index = 0);
	std::expected<std::vector<DataType>, script_error> get_var_range(std::string name);
	Type get_type(DataType val);
	void start();
private:
	std::unordered_map<std::string, std::vector<DataType>> variables;
	std::string path;
private:
	class Parser
	{
	public:
		Parser(ScriptEngine& eng);
		void operator()(std::string path);
	private:
		const bool is_key_word(const std::string word) const;
		const std::expected<DataType, script_error> parse_type(const std::string word) const;
		// This helper function checks if a string is all numeric or not
		const bool is_num(const std::string& str) const;
	private:
		Code code;
		ScriptEngine& eng;
	};
};