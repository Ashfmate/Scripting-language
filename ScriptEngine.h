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
#include <format>

#pragma endregion

std::string to_lower(std::string word);

class ScriptError
{
public:
	enum Errors : int
	{
		Var_Not_Exist,
		Out_Of_Range,
		Type_Not_Supported,
		Invalid_Statement,
		Assign_Operator_Missing,
		Missing_Quote
	};
public:
	ScriptError() = default;
	ScriptError(std::string msg, Errors err_code) : err_msg(msg), err_code(err_code) { }
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
	std::expected<ScriptEngine*, ScriptError> set_var(const std::string name, const DataType value, const size_t index = 0);
	std::expected<ScriptEngine*, ScriptError> set_var(const std::string name, const std::vector<DataType> value);
	ScriptEngine& app_var(const std::string name, const DataType value);
	ScriptEngine& app_var(const std::string name, const std::vector<DataType> value);
	const std::expected<DataType, ScriptError> get_var(const std::string name, const size_t index = 0);
	const std::expected<std::vector<DataType>, ScriptError> get_var_range(const std::string name);
	const Type get_type(const DataType val) const;
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
		const std::expected<void, ScriptError> pick_statment(const std::string line);
		const std::expected<void, ScriptError> create_var(std::istringstream& line);
	private:
		const std::expected<std::string, ScriptError> parse_quotes(std::string first_word, std::istringstream& line);
		const std::vector<std::string>::iterator find_key_word(const std::string word);
		const std::expected<DataType, ScriptError> parse_type(const std::string word) const;
		// This helper function checks if a string is all numeric or not
		const bool is_num(const std::string& str) const;
	private:
		std::optional<std::vector<std::string>> keywords;
		ScriptEngine& eng;
	};
};