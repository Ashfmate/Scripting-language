export module Parser;

import <memory>;
import <optional>;
import <vector>;
import <variant>;
import <ranges>;
import <fstream>;
import <sstream>;
import <vector>;
import <string>;
import Error;
import Utils;
import Code;

using std::vector, std::string;

namespace
{
	vector<string> var_names;

	typedef std::pair<vector<string>, code::StatementType> CodeStatement;
	typedef vector<CodeStatement> CodeLine;
	typedef vector<CodeLine> CodeBlock;

	/// <summary>
	/// Innermost layer of the parsing process
	/// </summary>
	/// <param name="line"> The line of code (i.e text separated by new lines)</param>
	/// <returns> Returns a vector of tokens </returns>
	const err::Expect<vector<string>> parseLine(const string& line);
	/// <summary>
	/// Middle layer of the parsing process
	/// </summary>
	/// <param name="tokens"> The result of parseLine function </param>
	/// <returns> 
	/// Returns a CodeLine which is a vector of pairs of tokens and statement type
	/// </returns>
	const err::Expect<CodeLine> groupStatements(const vector<string>& tokens);
	/// <summary>
	/// Helper function to check if a variable exists or not.
	/// If the string has indexing braces then it will return the index too as an optional
	/// </summary>
	/// <param name="var"> 
	/// The name of the variable in question, could also have index braces and will still identify the variable
	/// </param>
	/// <returns> 
	/// Whether the boolean exists or not as well as the index if provided 
	/// </returns>
	const err::Expect<std::pair<bool, std::optional<int>>> findVariable(string var);
	/// <summary>
	/// Gets the type of value from a string (MUST BE USED AFTER findVariable, otherwise will return Undefined
	/// </summary>
	/// <param name="val"> The string to look through </param>
	/// <returns> The Type of value that it is </returns>
	const err::Expect<code::Type> getType(const string& val);
	/// <summary>
	/// Helper function for groupStatements Which goes through the tokens and creates the individual Statements
	/// </summary>
	/// <param name="tokens"> The tokens to go through </param>
	/// <param name="index"> There to modify the groupStatement function index </param>
	/// <returns> Individual Statements </returns>
	const err::Expect<CodeStatement> makeStatement(const vector<string>& tokens, size_t& index);
	/// <summary>
	/// Converts an input stream (file or std::cin) into a string
	/// </summary>
	/// <param name="input"> The input stream to be gotten </param>
	/// <returns> A string representation of the input stream </returns>
	const string fileToString(std::istream& input);
	/// <summary>
	/// Splits the string of multiple lines into multiple strings
	/// </summary>
	/// <param name="lines"> String that contains newline characters</param>
	/// <returns> An array of strings that are split by newlines </returns>
	const vector<string> splitNewLine(const string& lines);
	/// <summary>
	/// Groups The multiple lines of code after they have been checked and parsed through (TODO make another function that allows for console input)
	/// </summary>
	/// <param name="path"> The path of the text file to use </param>
	/// <returns> The block of code generated </returns>
	const err::Expect<CodeBlock> groupLines(const string& path);

	/////////////////////////////////////////////////////////////////////////////
	/////						IMPLEMENTATION								/////
	/////////////////////////////////////////////////////////////////////////////


}