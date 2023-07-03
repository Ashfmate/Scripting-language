#include "Parser.h"

Parser::Parser(ScriptEngine& eng) : eng(eng)
{
	key_words.append_range(vector<string>({ "if","elf","else","print","println" }));
}

std::expected<vector<Code>, ScriptError> Parser::operator()(string path)
{
	std::ifstream file(path);
	string line;
	while (std::getline(file, line, '\n'))
	{
		if (line.empty()) continue;
		else if (line == "exit") break;
		else if (auto& cur = pick_statment(line); !cur)
			return std::unexpected(cur.error());
		else
		{
			code_lines.emplace_back(*cur.value());
		}
	}
	return std::move(code_lines);
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::pick_statment(const string& line)
{
	auto tokens = stream_to_vector_converter(line);
	if (!tokens) return std::unexpected(tokens.error());
	if (auto& statement = find_statment(tokens.value()); statement)
	{
		std::unique_ptr<Code> pCode;
		switch (statement.value())
		{
		case Code::Var_Create:
			if (auto res = create_var(*tokens))
				pCode = std::move(res).value();
			else
				return std::unexpected(res.error());
			break;
		case Code::Var_Set:
			if (auto res = set_var(*tokens))
				pCode = std::move(res).value();
			else
				return std::unexpected(res.error());
			break;
		case Code::If:
			break;
		case Code::Else_If:
			break;
		case Code::Else:
			break;
		case Code::While:
			break;
		case Code::Iterate:
			break;
		case Code::Print:
		case Code::Println:
			if (auto res = print(*tokens, statement.value() == Code::Println))
				pCode = std::move(res).value();
			else
				return std::unexpected(res.error());
			break;
		case Code::Function:
			break;
		}
		return std::move(pCode);
	}
	else
		return std::unexpected(ScriptError("The line { " + line + " } is not a valid statement", Invalid_Statement));
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::create_var(vector<string> tokens)
{
	auto pCode = std::make_unique<Code>();
	pCode->name = tokens[0];
	pCode->type = Code::Var_Create;
	pCode->data_types = std::make_shared<vector<DataType>>();
	pCode->ret_val = pCode->data_types;
	for (size_t i = 2; i < tokens.size(); ++i)
	{
		if (auto& res = parse_type(tokens[i]))
			pCode->data_types->emplace_back(res.value());
		else
			return std::unexpected(res.error());
	}
	return std::move(pCode);
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::set_var(vector<string> tokens)
{
	auto pCode = std::make_unique<Code>();
	int index = 0;
	string name = tokens[0];
	if (size_t l_bracket = name.find('['), r_bracket = name.find(']'); l_bracket != string::npos)
	{
		try
		{
			index = std::stoi(name.substr(l_bracket + 1, r_bracket));
		}
		catch (...)
		{
			return std::unexpected(ScriptError("Index must be an integer", Index_Not_Integral));
		}
		name = name.substr(0, l_bracket);
	}
	else if (auto it = std::ranges::find(tokens, "at"); it != tokens.end())
	{		
		++it;
		try
		{
			index = std::stoi(*it);
		}
		catch (...)
		{
			return std::unexpected(ScriptError("Index must be an integer", Index_Not_Integral));
		}
	}

	auto& var = find_variable(name);
	size_t val_pos = std::ranges::distance(tokens.begin(), std::ranges::find(tokens, "=")) + 1;
	vector<string> vals(tokens.begin() + val_pos, tokens.end());
	auto& ans = parse_values(vals);
	if (!ans)
		return std::unexpected(ans.error());
	
	
	if (index > pCode->data_types->size())
		return std::unexpected(ScriptError("Index is out of range", Out_Of_Range));

	pCode->data_types = var->data_types;
	pCode->ret_val = std::make_shared<vector<DataType>>(ans.value());

	for (size_t i = (size_t)index; i < ans.value().size(); ++i)
	{
		if (i < pCode->data_types->size())
			pCode->data_types->at(i) = ans.value()[i];
		else
			pCode->data_types->emplace_back(ans.value()[i]);
	}
	pCode->name = var->name;
	pCode->type = Code::Var_Set;
	return std::move(pCode);
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::print(const vector<string>& tokens, bool new_line)
{
	std::unique_ptr<Code> pCode = std::make_unique<Code>();
	pCode->type = Code::Print;
	if (new_line)
		pCode->type = Code::Println;
	pCode->data_types = std::make_shared<vector<DataType>>();
	pCode->ret_val = pCode->data_types;
	vector<string> vals(tokens.begin() + 1, tokens.end());
	if (auto& ans = parse_values(vals); !ans)
		return std::unexpected(ans.error());
	else
		pCode->data_types->append_range(ans.value());
	// Print : data_types (the values to set to)
	return std::move(pCode);
}

const bool Parser::is_key_word(const string word) const
{
	return std::ranges::contains(key_words, word);
}

const std::expected<DataType, ScriptError> Parser::parse_type(const string word) const
{
	if (auto wrd = to_lower(word); wrd == "null")
		return std::nullopt;
	else if (is_num(wrd))
	{
		if (wrd.find_first_of('.') == string::npos)
		{
			return std::stoi(wrd);
		}
		else
			return std::stod(wrd);
	}
	else if (wrd == "true" || wrd == "false")
		return wrd == "true";
	else if (wrd.front() == wrd.back() && wrd.front() == '\"')
		return wrd.substr(1, wrd.size() - 2);
	else
		return std::unexpected(ScriptError("The value given is not supported", Type_Not_Supported));
}

const bool Parser::is_num(const string& str) const
{
	if (str.empty()) return false;

	bool has_decimal = false;
	size_t i = 0;

	if (str[0] == '-') i = 1;

	for (; i < str.size(); ++i)
	{
		if (std::isdigit(str[i])) continue;

		else if (str[i] != '.') return false;  // invalid character

		else if (has_decimal) return false;

		has_decimal = true;
	}

	return true;
}

const std::expected<vector<DataType>, ScriptError> Parser::parse_values(vector<string>& tokens)
{
	vector<DataType> var_col;
	for (auto& cur_val : tokens)
	{
		// If the user inputs an unnecessary comma
		if (cur_val == ",") continue;
		// If the user inputs a comma right next to the value (also unnecessary)
		if (cur_val.back() == ',') cur_val.pop_back();
		// If the user inputs an existing variable
		if (auto it = find_variable(cur_val); it != code_lines.end())
		{
			var_col.append_range(*std::move(it->data_types));
		}
		// This will check if the value entered is reckognizable
		else
		{
			auto& val = parse_type(to_lower(cur_val));
			if (!val) return std::unexpected(val.error());
			var_col.emplace_back(val.value());
		}
	}
	return var_col;
}

const vector<Code>::iterator Parser::find_variable(const std::string_view& name)
{
	return std::ranges::find_if(code_lines, [&name](const Code& code)
		{
			return code.name == name;
		});
}

const std::expected<vector<string>, ScriptError> Parser::stream_to_vector_converter(const std::string& line) const
{
	std::istringstream line_stream(line);
	string token;
	vector<string> res;
	string full_quote;
	bool in_quote = false;
	while (line_stream >> token)
	{
		in_quote = in_quote || std::ranges::contains(token, '\"');
		if (in_quote)
		{
			if (token.back() == '\"')
			{
				in_quote = false;
				full_quote += token;
				res.emplace_back(full_quote);
			}
			else
			{
				full_quote += token + " ";
			}
		}
		else
			res.emplace_back(token);
	}
	if (in_quote) return std::unexpected(ScriptError("There is no closing quotes", Missing_Quote));
	return res;
}

const std::expected<Code::Statement_Type, ScriptError> Parser::find_statment(const vector<string>& tokens)
{
	if (tokens.empty())
		return Code::Empty_Statement;
	else if (std::string first_statement = to_lower(tokens[0]); first_statement == "print")
		return Code::Print;
	else if (first_statement == "println")
		return Code::Println;
	else
	{		
		size_t l_bracket = first_statement.find('[');
		if (l_bracket != string::npos)
		{
			// Check to see if the variable exists, otherwise, it is not possible to index an unknown variable
			if (find_variable(first_statement.substr(0, l_bracket)) == code_lines.end())
				return std::unexpected(ScriptError("Cannot index an unkown variable", Var_Not_Exist));
			// Look for the right bracket after the left bracket
			size_t r_bracket = first_statement.find(']', l_bracket + 1);
			// If there is no right bracket after the left bracket
			if (r_bracket == string::npos)
				return std::unexpected(ScriptError("Either right bracket is missing or before left bracket", Incorrect_Statement));
			// The left and right backets are right next to each other and that breaks structure
			if ((l_bracket - r_bracket) == 1)
				return std::unexpected(ScriptError("Index is not provided", Incomplete_Statement));
			// After all the tests, return the result
			return Code::Var_Set;
		}
		if (find_variable(first_statement.substr(0, l_bracket)) == code_lines.end())
		{
			if (tokens.size() <= 2)
				return std::unexpected(ScriptError("Not enought to create a variable", Incomplete_Statement));
			if (tokens[1] != "=")
				return std::unexpected(ScriptError("unreckognized token, second token must be (=)", Incorrect_Statement));
			return Code::Var_Create;
		}
		else
		{
			if (tokens.size() <= 2)
				return std::unexpected(ScriptError("Not enought to set a variable", Incomplete_Statement));
			if (tokens[1] != "at")
			{
				if (tokens[1] != "=")
					return std::unexpected(ScriptError("unreckognized token, second token must be (=) or (at)", Incorrect_Statement));
				return Code::Var_Set;
			}
			if (tokens.size() <= 4)
				return std::unexpected(ScriptError("Not enought to set a variable with at key word", Incomplete_Statement));
			if (tokens[3] != "=")
				return std::unexpected(ScriptError("unreckognized token, third token must be (=)", Incorrect_Statement));
			return Code::Var_Set;
		}
	}
}

string to_lower(string word)
{
	std::for_each(word.begin(), word.end(), ::tolower);
	return word;
}

