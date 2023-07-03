#pragma once
#include "ScriptEngine.h"
// A class specialized in parsing through the file to come up with the executable code (coming soon where it returns a code struct)
class Parser
{
public:
	// @param eng The engine will be passed in by reference because it cannot use outer engine class
	Parser(ScriptEngine& eng);
	// Starts the parser, idk made it with an operator because I am cool like that
	// @param path The path to the file to execute
	std::expected<vector<Code>, ScriptError> operator()(string path);

#pragma region Worker_Functions

private:
	// Chooses which statement to do
	// @param line The line of code to look a statement for
	// @return Returns either the code picked or an error
	const std::expected<std::unique_ptr<Code>, ScriptError> pick_statment(const vector<string>& tokens);
	/// <summary>
	/// Creates a variable
	/// </summary>
	/// <param name="tokens"> the collection of tokens that make up the variable creation statement </param>
	/// <returns> returns a unique pointer to the code generated or an error </returns>
	const std::expected<std::unique_ptr<Code>, ScriptError> create_var(vector<string> tokens);
	/// <summary>
	/// Sets a variable
	/// </summary>
	/// <param name="tokens"> the collection of tokens that make up the variable setting statement </param>
	/// <returns> returns a unique pointer to the code generated or an error </returns>
	const std::expected<std::unique_ptr<Code>, ScriptError> set_var(vector<string> tokens);
	const std::expected<std::unique_ptr<Code>, ScriptError> ret_var(vector<string> tokens);
	const std::expected<std::unique_ptr<Code>, ScriptError> print(const vector<string>& tokens, bool new_line);

#pragma endregion

#pragma region Helper_Parser_Functions

private:
	/// <summary>
	/// Helper function that returns whether the word is a keyword or not
	/// </summary>
	/// <param name="word"> The word in question </param>
	/// <returns> True if it exists or false if otherwise </returns>
	const bool is_key_word(const string word) const;
	// Helper function that will detect what type the value is from the line of code
	// @param word The value in question
	// @return Returns the data type or an error
	const std::expected<DataType, ScriptError> parse_type(const string word) const;
	// This helper function checks if a string is all numeric or not
	// @param str The string of possibly numbers
	const bool is_num(const string& str) const;
	// This helper function parses through the variables, constants and variables to give a vector of these values
	const std::expected<vector<DataType>, ScriptError> parse_values(vector<string>& tokens);
	// Helpful function for when to check if the variable exists or not
	const std::expected<std::pair<vector<Code>::iterator, std::optional<int>>, ScriptError> find_variable(std::string name);
	// Converts a stream to a vector of tokens
	const std::expected<vector<string>, ScriptError> stream_to_vector_converter(const std::string& line) const;
	const std::expected<Code::Statement_Type, ScriptError> find_statment(const vector<string>& tokens);
	bool str_to_int(const string& num, int& val) const;

#pragma endregion

#pragma region Parser_Member_Function

private:
	// Holds the collection of keywords
	vector<Code> code_lines;
	// Holds the keywords for the special functions (or for normal functions, but later)
	vector<string> key_words;
	// The engine which is the outer class
	ScriptEngine& eng;
};
#pragma endregion