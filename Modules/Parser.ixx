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
import Error;
import Utils;
import Code;

// TODO might change this implementation as a class but we will see

namespace
{
	vector<string> key_words{"if", "elf", "else", "while", "iterate"};
	vector<string> var_names;

	/// <summary>
	/// Innermost layer of the parsing process
	/// </summary>
	/// <param name="line"> The line of code (i.e text separated by new lines)</param>
	/// <returns> Returns a vector of tokens </returns>
	const err::Expect<vector<string>> parseLine(const std::string_view line);

	/// <summary>
	/// Generates a statement (The crux of the parsing engine)
	/// </summary>
	class StatementGenerator
	{
	public:
		/// <param name="tokens"> The tokens to look through and generate a statement from </param>
		/// <param name="index"> It is so that the outer function (groupStatement) can adapt to it</param>
		StatementGenerator(const vector<string>& tokens, size_t& index) : tokens(tokens) , index(index){}
		/// <summary>
		/// Starts the generation process
		/// </summary>
		/// <returns> An executable statement </returns>
		const err::Expect<code::ExecutableStatement> operator()();
	private:
		/// <summary>
		/// Loops through the tokens and picks out the values
		/// </summary>
		const err::ErrorCode loopValues();
		/// <summary>
		/// Handles the token if it is "print" or "println", will set handled to true if it is successful
		/// </summary>
		void handlePrint();
		/// <summary>
		/// Handles the token if it is a key word, will set handled to true if it is successful
		/// </summary>
		void handleKeyWord();
		/// <summary>
		/// Handles collection of arguments, will set handled to true if it is successful
		/// </summary>
		err::ErrorCode handleArgCollection();
		/// <summary>
		/// Handles Var structure, if it is a lone variable will set handled to true 
		/// Also checks variable name being apha numeric
		/// </summary>
		err::ErrorCode handleVarStructure();
		/// <summary>
		/// Handles assigning variables to values, will set handled to true if it is successful
		/// </summary>
		/// <param name="var_exist"> indicating if a variable exists or not </param>
		err::ErrorCode handleVarAssign(bool var_exist);
		/// <summary>
		/// Helper function where it takes a variable name and check if it is alpha numeric
		/// </summary>
		/// <param name="var"> A view to the variable name </param>
		/// <returns> Whether the variable name is a alpha numeric or not </returns>
		const bool isAlphaNum(const std::string_view var);
	private:
		vector<string> statement;
		code::StatementType type = code::StatementType::Empty_Statement;
		const vector<string>& tokens;
		size_t& index;
		bool handled = false;
	};
	/// <summary>
	/// Middle layer of the parsing process
	/// </summary>
	/// <param name="tokens"> The result of parseLine function </param>
	/// <returns> 
	/// Returns a CodeLine which is a vector of pairs of tokens and statement type
	/// </returns>
	const err::Expect<code::ExecutableLine> groupStatements(const vector<string>& tokens);
	/// <summary>
	/// Converts an std::cin into a string, will exit when the character (\) is present
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
	const vector<string> splitNewLine(const std::string_view line_block);
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
			index = utils::getNumType(var.substr(l_brace + 1, r_brace - l_brace - 1));
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

		auto it = std::remove_if(lines.begin(), lines.end(), [](const string& line)
			{
				return line.empty();
			});
		lines.erase(it, lines.end());

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

		auto it = std::remove_if(lines.begin(), lines.end(), [](const string& line)
			{
				return line.empty();
			});
		lines.erase(it, lines.end());
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
	
	const err::Expect<vector<string>> parseLine(const std::string_view line)
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
					utils::moveToken(tokens, token);
			}
			else if (in_quote)
				token.push_back(ch);
			else if (ch == ' ')
				utils::moveToken(tokens, token);
			else
				token.push_back(ch);
		}
		if (in_quote) return std::unexpected(err::ErrorCode::Missing_Quote);
		utils::moveToken(tokens, token);
		return tokens;
	}

	// Statement Generator function definition START


	const err::Expect<code::ExecutableStatement> StatementGenerator::operator()()
	{
		// This error should not happen but if it does, it will be handled
		if (tokens.size() - index == 0)
			return std::unexpected(err::ErrorCode::Empty_Statement);
		bool var_exist = false;
		if (auto var = parse::findVariable(tokens[index]))
			var_exist = var.value().first;
		else
			return std::unexpected(var.error());
		if (!handled)
		{
			// Handles printing statements
			handlePrint();
		}
		else if (!handled)
		{
			// Handles key word look up
			handleKeyWord();
		}
		else if (!handled)
		{
			// Handles raw value argument collection
			if (auto res = handleArgCollection(); res != err::ErrorCode::No_Error)
				return std::unexpected(res);
		}
		else if (!handled)
		{
			// Handles checking if the variable is aphanumeric and if it is a lone variable
			if (auto res = handleVarStructure(); res != err::ErrorCode::No_Error)
				return std::unexpected(res);
		}
		else if (!handled)
		{
			// Handles assigning to a variable
			if (auto res = handleVarAssign(var_exist); res != err::ErrorCode::No_Error)
				return std::unexpected(res);
		}
		else if (!handled)
		{
			throw 0;
		}
		return code::ExecutableStatement{std::move(statement), type};
	}

	const err::ErrorCode StatementGenerator::loopValues()
	{
		while (index + 1 < tokens.size())
		{
			++index;
			auto var = parse::findVariable(tokens[index]);
			if (!var) return var.error();
			else if (auto type = parse::getType(tokens[index]);
				var.value().first || type)
				statement.emplace_back(tokens[index]);
			else
				return type.error();
		}
		return err::ErrorCode::No_Error;
	}

	void StatementGenerator::handlePrint()
	{
		if (tokens[index] == "print")
		{
			statement.emplace_back(tokens[index]);
			type = code::StatementType::Print;
			handled = true;
		}
		// If the user inputted "println"
		else if (tokens[index] == "println")
		{
			statement.emplace_back(tokens[index]);
			type = code::StatementType::Println;
			handled = true;
		}
	}

	void StatementGenerator::handleKeyWord()
	{
		handled = std::ranges::contains(key_words, tokens[index]);
	}

	err::ErrorCode StatementGenerator::handleArgCollection()
	{
		if (auto var_type = parse::getType(tokens[index]))
		{
			--index;
			if (auto res = loopValues(); res != err::ErrorCode::No_Error)
				return res;
			type = code::StatementType::Arg_Col;
			handled = true;
		}
		return err::ErrorCode::No_Error;
	}

	err::ErrorCode StatementGenerator::handleVarStructure()
	{
		statement.emplace_back(tokens[index]);
		if (!isAlphaNum(tokens[index]))
			return err::ErrorCode::Var_Not_AlphaNumeric;

		if (tokens.size() - index == 1)
		{
			statement.emplace_back(tokens[index]);
			type = code::StatementType::Var_Ret;
			handled = true;
		}
		return err::ErrorCode::No_Error;
	}

	err::ErrorCode StatementGenerator::handleVarAssign(bool var_exist)
	{
		if (tokens[++index] != "=")
			return err::ErrorCode::Assign_Not_Exist;

		else if (tokens.size() - index == 1)
			return err::ErrorCode::Values_Not_Exist;
		statement.emplace_back(tokens[index]);

		if (auto res = loopValues(); res != err::ErrorCode::No_Error)
			return res;

		if (var_exist)
			type = code::StatementType::Var_Set;
		else
		{
			type = code::StatementType::Var_Create;
			var_names.emplace_back(statement.front());
		}
		handled = true;
		return err::ErrorCode::No_Error;
	}

	const bool StatementGenerator::isAlphaNum(const std::string_view var)
	{
		return std::all_of(var.begin(), var.end(), std::isalnum);
	}

	// Statement Generator function definition END

	const err::Expect<code::ExecutableLine> groupStatements(const vector<string>& tokens)
	{
		code::ExecutableLine code_line;
		for (size_t i = 0; i < tokens.size(); ++i)
		{
			if (auto line = StatementGenerator{tokens,i}())
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
				res.push_back(ch);
			else break;
		}
		return res;
	}

	const string streamToString(std::ifstream& input)
	{
		std::stringstream sstream;
		sstream << input.rdbuf();
		return std::move(sstream).str();
	}

	const vector<string> splitNewLine(const std::string_view line_block)
	{
		vector<string> lines;
		lines.reserve(utils::estimateSize(line_block, "\n"));

		std::ranges::transform(
			std::ranges::subrange(line_block) | std::views::split('\n'),
			std::back_inserter(lines),
			[](auto&& view) { return std::string(view.begin(), view.end()); });

		return lines;
	}
}

