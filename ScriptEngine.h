#pragma region Preprocessor

#pragma once
#include <variant>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <optional>
#include <expected>
#include <algorithm>
#include <memory>
#include <fmt\core.h>
#include <fmt\ranges.h>
#include "ScriptError.h"

#pragma endregion

using std::string, std::vector;
// helpful function for when word needs to be in lowercase
string to_lower(string word);


// Simplification for this massive optional of a variant of bool, int, double, string
using DataType = std::optional<std::variant<bool, int, double, string>>;
// This is a struct the defines what a code is
struct Code
{
	enum Statement_Type
	{
		Empty_Statement,
		Var_Create,
		Var_Set,
		If,
		Else_If,
		Else,
		While,
		Iterate,
		Print,
		Println,
		Function
	};
	std::shared_ptr<vector<DataType>> data_types;
	std::shared_ptr<vector<DataType>> ret_val;
	std::optional<string> name;
	Statement_Type type = Empty_Statement;
	vector<Code> inner_code;
};


// The engine that runs everything
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
	// The function that starts the engine
	ScriptError start(string path);
	ScriptError exec();
	// Takes the value and returns an enum that indicates the value's data type
	// @param val The value in question
	// @return Returns an enum to indicate the value's data type
	const Type get_type(const DataType val) const;
private:
	vector<Code> code_lines;
private:

};