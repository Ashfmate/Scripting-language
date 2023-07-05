export module Parser;

#include <sstream>
#include <fstream>
#include <algorithm>;
#include <ranges>;

import <memory>;
import <optional>;
import <vector>;
import <variant>;
import <vector>;
import <string>;
import Error;
import Utils;
import Code;

using std::vector, std::string;

namespace
{
	vector<string> key_words{"if", "elf", "else", "while", "iterate"};
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
	/// Middle layer of the parsing process
	/// </summary>
	/// <param name="tokens"> The result of parseLine function </param>
	/// <returns> 
	/// Returns a CodeLine which is a vector of pairs of tokens and statement type
	/// </returns>
	const err::Expect<CodeLine> groupStatements(const vector<string>& tokens);
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


	const err::Expect<vector<string>> parseLine(const string& line)
	{
		vector<string> tokens;
		tokens.reserve(utils::estimateSize(line, " \n;"));
		string token;
		bool in_quote = false;
		for (char ch : line)
		{
			if (ch == '\"')
			{
				in_quote = !in_quote;
				token.push_back(ch);
				if (!in_quote)
					utils::emplace(tokens, token);
			}
			else if (in_quote)
				token.push_back(ch);
			else if (ch == ' ')
				utils::emplace(tokens, token);
			else
				token.push_back(ch);
		}
		if (in_quote) return std::unexpected(err::ErrorCode::Missing_Quote);
		utils::emplace(tokens, token);
		return std::move(tokens);
	}

	const err::Expect<std::pair<bool, std::optional<int>>> findVariable(string var)
	{
		// Resulting variables
		bool is_found = false;
		std::optional<int> index = std::nullopt;
		// Checks if there is no indexing
		if (auto l_brace = var.find('['); l_brace == string::npos)
			is_found = std::ranges::contains(var_names, var);
		// There appears to be indexing
		else
		{
			auto r_brace = var.find(']', l_brace + 1);
			if (r_brace == string::npos)
				return std::unexpected(err::ErrorCode::Missing_Right_Brace);
			else if (r_brace - l_brace < 2)
				return std::unexpected(err::ErrorCode::Empty_Index_Brace);
			index = utils::toInt(var.substr(l_brace + 1, r_brace - l_brace - 1));
			var = var.substr(0, l_brace);
			is_found = std::ranges::contains(var_names, var);
			if (!index)
				return std::unexpected(err::ErrorCode::Non_Integral_Index);
			else if (!is_found)
				return std::unexpected(err::ErrorCode::Indexing_Unknown_Var);
			else if (index.value() < 0)
				return std::unexpected(err::ErrorCode::Non_Positive_Index);
		}
		return std::pair<bool, std::optional<int>>{ is_found, index };
	}

	const err::Expect<code::Type> getType(const string& val)
	{
		if (val == "true" || val == "false")
			return code::Type::Boolean;
		else if (val == "null")
			return code::Type::Null;
		else if (auto type = utils::isNum(val); type == utils::NumType::Int)
			return code::Type::Int;
		else if (type == utils::NumType::Double)
			return code::Type::Double;
		else if (val.front() == val.back() && val.back() == '\"')
			return code::Type::String;
		else
			return std::unexpected(err::ErrorCode::Unsupported_Type);
	}

	const err::Expect<CodeStatement> makeStatement(const vector<string>& tokens, size_t& index)
	{
		// This error should not happen but if it does, it will be handled
		if (tokens.empty())
			return std::unexpected(err::ErrorCode::Empty_Statement);
		// The result variables
		vector<string> statement{};
		code::StatementType type = code::StatementType::Empty_Statement;
		// If the user inputted "print"
		if (tokens[index] == "print")
		{
			statement.emplace_back(tokens[index]);
			type = code::StatementType::Print;
		}
		// If the user inputted "println"
		else if (tokens[index] == "println")
		{
			statement.emplace_back(tokens[index]);
			type = code::StatementType::Println;
		}
		// If there was something wrong when looking for a variable
		else if (auto var = findVariable(tokens[index]); !var)
			return std::unexpected(var.error());
		// If variable look up was successful
		else if (!std::ranges::contains(key_words, tokens[index]))
		{
			std::string new_var;
			if (!var.value().first)
				new_var = tokens[index];
			if (!var.value().first && 
				std::ranges::all_of(tokens[index],[](const char ch) 
				{ return std::isalnum(ch); }))
				return std::unexpected(err::ErrorCode::Var_Not_AlphaNumeric);

			if (tokens.size() - index == 1)
			{
				statement.emplace_back(tokens[index]);
				type = code::StatementType::Var_Ret;
			}
			else if (tokens[++index] != "=")
				return std::unexpected(err::ErrorCode::Assign_Not_Exist);
			else if (tokens.size() - index == 1)
				return std::unexpected(err::ErrorCode::Values_Not_Exist);
			while (++index < tokens.size())
			{
				auto var = findVariable(tokens[index]);
				if (!var) return std::unexpected(var.error());
				else if (auto data_type = getType(tokens[index]); var.value().first || data_type)
					statement.emplace_back(tokens[index]);
				else
					return std::unexpected(data_type.error());
			}
			if (var.value().first)
				code::StatementType::Var_Set;
			else
			{
				code::StatementType::Var_Create;
				var_names.emplace_back(new_var);
			}
		}
		return std::pair<vector<string>, code::StatementType>
		{std::move(statement), type};
	}

	const err::Expect<CodeLine> groupStatements(const vector<string>& tokens)
	{
		CodeLine code_line;
		for (size_t i = 0; i < tokens.size(); ++i)
		{
			if (auto line = makeStatement(tokens, i))
				code_line.emplace_back(line.value());
			else
				return std::unexpected(line.error());
		}
		return code_line;
	}

	const string fileToString(std::istream& input)
	{
		
	}

	const vector<string> splitNewLine(const string& lines)
	{
		return vector<string>();
	}

	const err::Expect<CodeBlock> groupLines(const string& path)
	{
		return err::Expect<CodeBlock>();
	}
}