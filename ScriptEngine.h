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
#include <fmt\core.h>
#include <fmt\ranges.h>

#pragma endregion

// helpful function for when word needs to be in lowercase
std::string to_lower(std::string word);

// Class for holding all error states and for passing the error message and their code
class ScriptError
{
public:
	enum Errors : int
	{
		No_Error,
		Var_Not_Exist,
		Out_Of_Range,
		Type_Not_Supported,
		Invalid_Statement,
		Assign_Operator_Missing,
		Missing_Quote,
		Code_Missing_Name,
		Code_Missing_Values
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
// Simplification for this massive optional of a variant of bool, int, double, string
using DataType = std::optional<std::variant<bool, int, double, std::string>>;

// This is a struct the defines what a code is
struct Code
{
	enum Statement_Type
	{
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
	std::vector<DataType> data_types;
	std::optional<std::vector<DataType>> ret_val;
	std::optional<std::string> name;
	Statement_Type type;
	std::vector<Code> inner_code;
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
	// @param path Defines the path of the file to interpret
	ScriptEngine(std::string path);
	// The function that starts the engine
	ScriptError start();
#pragma region Variable_Changing_Functions

private:
	// Sets a variable within the engine
	// @param name The name of the variable in question
	// @param value The type of value to be set (bool , int , double , string , std::nullopt gives null )
	// @param index Gives the ability to set the individual element of the variable
	// @return a Returns itself as a pointer (could not do reference) or an error should it arise
	std::expected<ScriptEngine*, ScriptError> set_var(const std::string name, const DataType value, const size_t index = 0);
	// Sets the range that a variable has within the engine
	// @param name The name of the variable in question
	// @param value The range of values to be set where each element may be (bool , int , double , string , std::nullopt gives null )
	// @return a Returns itself as a pointer (could not do reference) or an error should it arise
	std::expected<ScriptEngine*, ScriptError> set_var(const std::string name, const std::vector<DataType> value);
	// Appends a value to a variable within the engine
	// @param name The name of the variable in question
	// @param value The value to be appended (bool , int , double , string , std::nullopt gives null )
	// @return Returns itself as a reference
	ScriptEngine& app_var(const std::string name, const DataType value);
	// Appends a range of values to a variable within the engine
	// @param name The name of the variable in question
	// @param value The range of values to be appended where each element may be (bool , int , double , string , std::nullopt gives null )
	// @return Returns itself as a reference
	ScriptEngine& app_var(const std::string name, const std::vector<DataType> value);
	// Gets a value from a variable within the engine
	// @param name The name of the variable in question
	// @param index The index of the element that is held by the variable, leave this as default if the variable only has one element
	// @return Returns the value if the variable exists, else will return an error
	const std::expected<DataType, ScriptError> get_var(const std::string name, const size_t index = 0);
	// Gets the range of values from a variables within the engine
	// @param name The name of the variable in question
	// @return Returns the range of values if the variable exists, else will return an error
	const std::expected<std::vector<DataType>, ScriptError> get_var_range(const std::string name);
	// Takes the value and returns an enum that indicates the value's data type
	// @param val The value in question
	// @return Returns an enum to indicate the value's data type
	const Type get_type(const DataType val) const;

#pragma endregion

#pragma region Engine_Member_Variables

private:
	// Holds the variables in the engine, its key is string indicating the name of the variable and its value is a vector of DataType
	// which indicates the range of values that the variable name may have
	std::unordered_map<std::string, std::vector<DataType>> variables;
	// The path to the file
	std::string path;

#pragma endregion
private:
	// A class specialized in parsing through the file to come up with the executable code (coming soon where it returns a code struct)
	class Parser
	{
	public:
		// @param eng The engine will be passed in by reference because it cannot use outer engine class
		Parser(ScriptEngine& eng);
		// Starts the parser, idk made it with an operator because I am cool like that
		// @param path The path to the file to execute
		ScriptError operator()(std::string path);

#pragma region Worker_Functions

	private:
		// Chooses which statement to do
		// @param line The line of code to look a statement for
		// @return Returns either the code picked or an error
		const std::expected<std::unique_ptr<Code>, ScriptError> pick_statment(const std::string line);
		// The creating a variable statment
		// @param line The line that has the variable's name, the assign operator and the list of variables
		// @return Returns either the code done or an error
		const std::expected<std::unique_ptr<Code>, ScriptError> create_var(std::istringstream& line);
		const std::expected<std::unique_ptr<Code>, ScriptError> print(std::istringstream& line, bool new_line);
		// Executes the instructions given in the code_lines
		const ScriptError exec_code(const Code& code);

#pragma endregion

#pragma region Helper_Parser_Functions

	private:
		// Helper function that is used when there is string and we need the entire qoute of string
		// @param first_word Used for simplicity, it should hold the first word of the quoted string it will also be used as the return
		// @param line The line of code that would be used to fill in the quoted string
		// @return Returns the string that is incased in quotes or returns an error
		const std::expected<std::string, ScriptError> parse_quotes(std::string first_word, std::istringstream& line);
		// Helper function that returns whether the word is a keyword or not
		// @param word The word in question
		// @return True if it exists or false if otherwise
		const bool is_key_word(const std::string word) const;
		// Helper function that will detect what type the value is from the line of code
		// @param word The value in question
		// @return Returns the data type or an error
		const std::expected<DataType, ScriptError> parse_type(const std::string word) const;
		// This helper function checks if a string is all numeric or not
		// @param str The string of possibly numbers
		const bool is_num(const std::string& str) const;
		// This helper function parses through the variables, constants and variables to give a vector of these values
		const std::expected<std::vector<DataType>, ScriptError> parse_values(std::istringstream& line);
		// Helpful function for when to check if the variable exists or not
		const std::vector<Code>::iterator find_variable(const std::string& name);

#pragma endregion

#pragma region Parser_Member_Function

	private:
		// Holds the collection of keywords
		std::vector<Code> code_lines;
		// Holds the keywords for the special functions (or for normal functions, but later)
		std::vector<std::string> key_words;
		// The engine which is the outer class
		ScriptEngine& eng;
	};
#pragma endregion

};