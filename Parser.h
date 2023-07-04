#pragma once
#include "ScriptEngine.h"
/// <summary>
/// A class specialized in parsing through the file to come up with the executable code
/// </summary>
class Parser
{
public:
	/// <summary></summary>
	/// <param name="eng"> The engine will be passed in by reference because it cannot use outer engine class </param>
	Parser(ScriptEngine& eng);
	/// <summary>
	/// Is used to start the parsing part
	/// </summary>
	/// <param name="path"> The path of the file to parse through</param>
	/// <returns> Either the bunch of code that is made or an error</returns>
	std::expected<vector<Code>, ScriptError> operator()(string path);

#pragma region Worker_Functions

private:
	/// <summary>
	/// Chooses which statement to do
	/// </summary>
	/// <param name="tokens"> The line of code to look a statement for </param>
	/// <returns> Returns either the code picked or an error </returns>
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
	/// <summary>
	/// Returns a variable
	/// </summary>
	/// <param name="tokens"> The structure of returning a variable </param>
	/// <returns> Either the code built up for returning a variable or an error </returns>
	const std::expected<std::unique_ptr<Code>, ScriptError> ret_var(vector<string> tokens);
	/// <summary>
	/// Prints a variable, constant or a (later) result of an operation
	/// </summary>
	/// <param name="tokens"> The structure of printing </param>
	/// <param name="new_line"> Whether to add a new line or not</param>
	/// <returns> Either the code built up for printing or an error </returns>
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
	/// <summary>
	/// This helper function checks if a string is all numeric or not
	/// </summary>
	/// <param name="str"> The string of possibly numbers </param>
	/// <returns></returns>
	const bool is_num(const string& str) const;
	/// <summary>
	/// This helper function parses through the variables, constants and variables to give a vector of these values
	/// </summary>
	/// <param name="tokens"> The collection of values to look for each one's value or return type</param>
	/// <returns> Either the collection of values or an error</returns>
	const std::expected<vector<DataType>, ScriptError> parse_values(vector<string>& tokens);
	/// <summary>
	/// Helpful function for when to check if the variable exists or not
	/// </summary>
	/// <param name="name"> The name of the variable with or without index bracket (will change return)</param>
	/// <returns> 
	/// If the variable is found will return a pointer to the code_lines as an iterator. 
	/// If the index bracket is added will also initialize an optional integer as index.
	/// It may return an error depending on the structure of the index bracket
	/// </returns>
	const std::expected<std::pair<vector<Code>::iterator, std::optional<int>>, ScriptError> find_variable(std::string name);
	/// <summary>
	/// Converts a stream to a vector of tokens (TODO may make it separate strings that are not space-separated)
	/// </summary>
	/// <param name="line"> The string line of code from the file </param>
	/// <returns> The individual tokens as string</returns>
	const std::expected<vector<string>, ScriptError> stream_to_vector_converter(const std::string& line) const;
	/// <summary>
	/// Looks through the tokens to figure out what type of code it is
	/// </summary>
	/// <param name="tokens"> The structure given</param>
	/// <returns> Either the type of code or an error</returns>
	const std::expected<Code::Statement_Type, ScriptError> find_statment(const vector<string>& tokens);
	/// <summary>
	/// Does the same job as int.TryParse
	/// </summary>
	/// <param name="num"> The string to parse through</param>
	/// <returns>
	/// Returns a boolean as first indicating if it did parse.
	/// The second is the value it parsed (default int val if it couldn't)
	/// </returns>
	std::pair<bool, int> str_to_int(const string& num) const;

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