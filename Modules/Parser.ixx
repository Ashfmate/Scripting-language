#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ranges>
export module Parser;

import <memory>;
import <optional>;
import <vector>;
import <variant>;
import <vector>;
import <string>;
import Error;
import Utils;
import Code;

// TODO might change this implementation as a class but we will see

using std::vector, std::string;

namespace
{
	vector<string> key_words{"if", "elf", "else", "while", "iterate"};
	vector<string> var_names;

	/// <summary>
	/// Innermost layer of the parsing process
	/// </summary>
	/// <param name="line"> The line of code (i.e text separated by new lines)</param>
	/// <returns> Returns a vector of tokens </returns>
	const err::Expect<vector<string>> parseLine(const string& line);
	/// <summary>
	/// Helper function that loops through the values and fills the statement argument
	/// </summary>
	/// <param name="statement"> The built up statement </param>
	/// <param name="index"> The index to the token </param>
	const err::Expect<void> loopValues(const vector<string> tokens, const vector<string>& statement, size_t& index);
	/// <summary>
	/// Checks if the variable is alpha numeric
	/// </summary>
	/// <param name="var"> The variable in question </param>
	/// <returns> whether the variable is alpha numeric</returns>
	const bool checkAlNum(const string& var);
	/// <summary>
	/// Helper function for groupStatements Which goes through the tokens and creates the individual Statements
	/// </summary>
	/// <param name="tokens"> The tokens to go through </param>
	/// <param name="index"> There to modify the groupStatement function index </param>
	/// <returns> Individual Statements </returns>
	const err::Expect<code::CodeStatement> makeStatement(const vector<string>& tokens, size_t& index);
	/// <summary>
	/// Middle layer of the parsing process
	/// </summary>
	/// <param name="tokens"> The result of parseLine function </param>
	/// <returns> 
	/// Returns a CodeLine which is a vector of pairs of tokens and statement type
	/// </returns>
	const err::Expect<code::CodeLine> groupStatements(const vector<string>& tokens);
	/// <summary>
	/// Converts an std::cin into a string
	/// </summary>
	/// <param name="input"> The input stream to be gotten </param>
	/// <returns> A string representation of the input stream </returns>
	const string streamToString(std::istream& input);
	/// <summary>
	/// Converts an std::cin into a 
	/// </summary>
	/// <param name="input"> The input stream to be gotten </param>
	/// <returns> A string representation of the input stream </returns>
	const string streamToString(std::ifstream& input);
	/// <summary>
	/// Splits the string of multiple lines into multiple strings
	/// </summary>
	/// <param name="line_block"> String that contains newline characters</param>
	/// <returns> An array of strings that are split by newlines </returns>
	const vector<string> splitNewLine(const string& line_block);
}

export namespace parse
{
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
	/// <summary>
	/// Parses through the file input and produces tokens for code execution
	/// </summary>
	/// <param name="path"> The path of the text file to use </param>
	/// <returns> The block of code generated </returns>
	const err::Expect<code::CodeBlock> parseCode(const string& path)
	{
		std::ifstream file(path);
		vector<string> lines =
			std::move(splitNewLine(std::move(streamToString(file))));

		code::CodeBlock block;
		for (auto& line : lines)
		{
			if (auto line_text = parseLine(line))
			{
				if (auto code_line = groupStatements(line_text.value()))
					block.emplace_back(code_line.value());
				else
					return std::unexpected(code_line.error());
			}
			else
				return std::unexpected(line_text.error());
		}
		return block;
	}
	/// <summary>
	/// Parses through the console input and produces tokens for code execution
	/// </summary>
	/// <returns></returns>
	const err::Expect<code::CodeBlock> parseCode()
	{
		vector<string> lines =
			std::move(splitNewLine(std::move(streamToString(std::cin))));

		code::CodeBlock block;
		for (auto& line : lines)
		{
			if (auto line_text = parseLine(line))
			{
				if (auto code_line = groupStatements(line_text.value()))
					block.emplace_back(code_line.value());
				else
					return std::unexpected(code_line.error());
			}
			else
				return std::unexpected(line_text.error());
		}
		return block;
	}
	/// <summary>
	/// Gets the type of value from a string (MUST BE USED AFTER findVariable, otherwise will return Undefined
	/// </summary>
	/// <param name="val"> The string to look through </param>
	/// <returns> The Type of value that it is </returns>
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
}

namespace
{
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
		return tokens;
	}

	const err::Expect<void> loopValues(const vector<string> tokens, vector<string>& statement, size_t& index)
	{
		while (index + 1 < tokens.size())
		{
			++index;
			auto var = parse::findVariable(tokens[index]);
			if (!var) return std::unexpected(var.error());
			else if (auto type = parse::getType(tokens[index]);
				var.value().first || type)
				statement.emplace_back(tokens[index]);
			else
				return std::unexpected(type.error());
		}
	}

	const bool checkAlNum(const string& var)
	{
		return std::none_of(var.begin(), var.end(), std::isalnum);
	}

	const err::Expect<code::CodeStatement> makeStatement(const vector<string>& tokens, size_t& index)
	{
		// This error should not happen but if it does, it will be handled
		if (tokens.size() - index == 0)
			return std::unexpected(err::ErrorCode::Empty_Statement);
		// The result variables
		vector<string> statement{};
		code::StatementType type = code::StatementType::Empty_Statement;
		auto var = parse::findVariable(tokens[index]);
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
		else if (!var)
			return std::unexpected(var.error());
		// If variable look up was successful
		else if (std::ranges::contains(key_words, tokens[index]));
		// If the value is not a variable and so no need to check assignment and other stuff
		else if (auto var_type = parse::getType(tokens[index]))
		{
			if (auto res = loopValues(tokens, statement, --index); !res)
				return std::unexpected(res.error());
			type = code::StatementType::Arg_Col;
		}
		else
		{
			// The value is a variable
			statement.emplace_back(tokens[index]);
			if (checkAlNum(tokens[index]))
				return std::unexpected(err::ErrorCode::Var_Not_AlphaNumeric);

			if (tokens.size() - index == 1)
			{
				statement.emplace_back(tokens[index]);
				type = code::StatementType::Var_Ret;
			}
			else
			{
				if (tokens[++index] != "=")
					return std::unexpected(err::ErrorCode::Assign_Not_Exist);

				else if (tokens.size() - index == 1)
					return std::unexpected(err::ErrorCode::Values_Not_Exist);
				statement.emplace_back(tokens[index]);

				loopValues(tokens, statement, index);
				if (var.value().first)
					type = code::StatementType::Var_Set;
				else
				{
					type = code::StatementType::Var_Create;
					var_names.emplace_back(statement.front());
				}
			}
		}
		return std::pair<vector<string>, code::StatementType>
		{std::move(statement), type};
	}

	const err::Expect<code::CodeLine> groupStatements(const vector<string>& tokens)
	{
		code::CodeLine code_line;
		for (size_t i = 0; i < tokens.size(); ++i)
		{
			if (auto line = makeStatement(tokens, i))
				code_line.emplace_back(line.value());
			else
				return std::unexpected(line.error());
		}
		return code_line;
	}

	const string streamToString(std::istream& input)
	{
		string res;
		while (input)
		{
			auto ch = input.get();
			if (ch != '\\')
				res.push_back(input.get());
			else break;
		}
		return res;
	}

	const string streamToString(std::ifstream& input)
	{
		std::stringstream sstream;
		sstream << input.rdbuf();
		return sstream.str();
	}

	const vector<string> splitNewLine(const string& line_block)
	{
		vector<string> lines;
		lines.reserve(utils::estimateSize(line_block, "\n"));
		for (auto view : line_block | std::views::split('\n'))
			lines.emplace_back(std::ranges::to<std::string>(view));
		return lines;
	}
}

