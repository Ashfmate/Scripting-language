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
		auto tokens = stream_to_vector_converter(line);
		if (!tokens)
			return std::unexpected(tokens.error());
		if (tokens.value().empty()) continue;
		else if (tokens.value().front() == "exit") break;
		else if (auto& cur = pick_statment(tokens.value()); !cur)
			return std::unexpected(cur.error());
		else
		{
			code_lines.emplace_back(*cur.value());
		}
	}
	return std::move(code_lines);
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::pick_statment(const vector<string>& tokens)
{
	if (auto& statement = find_statment(tokens); statement)
	{
		std::unique_ptr<Code> pCode;
		switch (statement.value())
		{
		case Code::Var_Ret:
			if (auto res = ret_var(tokens))
				pCode = std::move(res).value();
			else
				return std::unexpected(res.error());
			break;
		case Code::Var_Create:
			if (auto res = create_var(tokens))
				pCode = std::move(res).value();
			else
				return std::unexpected(res.error());
			break;
		case Code::Var_Set:
			if (auto res = set_var(tokens))
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
			if (auto res = print(tokens, statement.value() == Code::Println))
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
		return std::unexpected(ScriptError("The line { " + fmt::format("{}", tokens) + " } is not a valid statement", Invalid_Statement));
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::create_var(const vector<string> tokens)
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

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::set_var(const vector<string> tokens)
{
	auto pCode = std::make_unique<Code>();
	int index = 0;
	string name = tokens[0];
	if (auto it = std::ranges::find(tokens, "at"); it != tokens.end())
	{		
		++it;
		if (auto ans = str_to_int(*it); !ans.first)
			return std::unexpected(ScriptError("Index must be an integer", Index_Not_Integral));
		else
			index = ans.second;
	}

	auto& var = find_variable(name);
	if (var.value().second)
		index = var.value().second.value();
	
	if (index < 0)
		return std::unexpected(ScriptError("Index must be a positive number", Index_Not_Positive));

	size_t val_pos = std::ranges::distance(tokens.begin(), std::ranges::find(tokens, "=")) + 1;
	vector<string> vals(tokens.begin() + val_pos, tokens.end());
	auto& ans = parse_values(vals);
	if (!ans)
		return std::unexpected(ans.error());
	
	pCode->data_types = var.value().first->data_types;

	if (index > pCode->data_types->size())
		return std::unexpected(ScriptError("Index is out of range", Out_Of_Range));

	pCode->ret_val = std::make_shared<vector<DataType>>(ans.value());

	for (size_t i = 0; i < ans.value().size(); ++i)
	{
		if (index + i < pCode->data_types->size())
			pCode->data_types->at(i + index) = ans.value()[i];
		else
			pCode->data_types->emplace_back(ans.value()[i]);
	}
	pCode->name = var.value().first->name;
	pCode->type = Code::Var_Set;
	return std::move(pCode);
}

const std::expected<std::unique_ptr<Code>, ScriptError> Parser::ret_var(const vector<string> tokens)
{
	auto pCode = std::make_unique<Code>();
	int index = -1;
	string name = tokens[0];
	if (tokens.size() > 1)
	{
		if (auto res = str_to_int(tokens.back()); !res.first)
			return std::unexpected(ScriptError("Index must be an integer", Index_Not_Integral));
		else
			index = res.second;
	}

	auto& it = find_variable(name);
	if (!it) return std::unexpected(it.error());
	else if (it.value().second)
		index = it.value().second.value();
	pCode->name = it.value().first->name;
	pCode->data_types = it.value().first->data_types;
	pCode->type = Code::Var_Ret;
	if (index == -1)
		pCode->ret_val = pCode->data_types;
	else
	{
		pCode->ret_val = std::make_shared<vector<DataType>>();
		if (index >= pCode->data_types->size())
			return std::unexpected(ScriptError("Index is out of range", Out_Of_Range));
		pCode->ret_val->emplace_back(pCode->data_types->at(index));
	}
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

const std::expected<vector<DataType>, ScriptError> Parser::parse_values(const vector<string>& tokens)
{
	vector<DataType> var_col;
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		auto& cur_val = tokens[i];
		// If the user inputs an existing variable
		if (auto it = find_variable(cur_val); it && it.value().first != code_lines.end())
		{
			vector<string> inner_res;
			if (!it.value().second)
			{
				if (tokens.size() > i + 2)
				{
					if (tokens[i + 1] == "at")
					{
						inner_res.emplace_back(tokens[i + 1]);
						inner_res.emplace_back(tokens[i + 2]);
						i += 2;
					}
				}
			}
			inner_res.insert(inner_res.begin(), { cur_val });
			if (auto statement = pick_statment(inner_res))
				var_col.append_range(*std::move(statement.value()->ret_val));
			else
				return std::unexpected(statement.error());
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

const std::expected<std::pair<vector<Code>::iterator,std::optional<int>>, ScriptError> Parser::find_variable(std::string name)
{
	std::optional<int> idx = std::nullopt;
	if (size_t l_bracket = name.find('['), r_bracket = name.find(']', l_bracket); l_bracket != string::npos)
	{
		if (r_bracket == string::npos)
			return std::unexpected(ScriptError("Either right bracket is before left or missing", Incomplete_Statement));
		if (r_bracket - l_bracket == 1)
			return std::unexpected(ScriptError("Cannot index without a value", Incomplete_Statement));
		if (auto val = parse_type(name.substr(l_bracket + 1, r_bracket - l_bracket - 1)))
		{
			if (eng.get_type(val.value()) == ScriptEngine::Int)
				idx.emplace(std::get<int>(val.value().value()));
			else
				return std::unexpected(ScriptError("Index must be an integral", Index_Not_Integral));
		}
		else
			return std::unexpected(val.error());
		name = name.substr(0, l_bracket);
	}
	auto res = std::ranges::find_if(code_lines, [&name](const Code& code)
		{
			return code.name == name;
		});
	return std::pair<vector<Code>::iterator, std::optional<int>>(res,idx);
}

const std::expected<vector<string>, ScriptError> Parser::stream_to_vector_converter(const std::string& line) const
{
	void(*emplace)(vector<string>&, string&) = [](vector<string>& res, string& token)
	{
		if (!token.empty())
			res.emplace_back(std::move(token));
	};
	vector<string> res;
	size_t estimate_size = std::ranges::count_if(line, [](const char ch) { return ch == ' ' || ch == '=' || ch == '\"' || ch == ','; });
	res.reserve(estimate_size);
	string token;
	bool in_quote = false;
	for (char ch : line)
	{
		if (ch == '\"')
		{
			in_quote = !in_quote;
			token.push_back(ch);
			if (!in_quote)
				emplace(res, token);
		}
		else if (in_quote)
			token.push_back(ch);
		else if (ch == ' ' || ch == ',')
			emplace(res, token);
		else if (ch == '=')
		{
			emplace(res, token);
			token.push_back(ch);
			emplace(res, token);
		}
		else
			token.push_back(ch);
	}
	if (in_quote) return std::unexpected(ScriptError("There is no closing quotes", Missing_Quote));

	if (!token.empty())
		emplace(res, token);

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
		
		if (auto it = find_variable(first_statement); !it)
		{
			return std::unexpected(it.error());
		}
		else if (auto it = find_variable(first_statement); it.value().first == code_lines.end())
		{
			if (tokens.size() <= 2)
				return std::unexpected(ScriptError("Not enought to create a variable", Incomplete_Statement));
			if (tokens[1] != "=")
				return std::unexpected(ScriptError("unreckognized token, second token must be (=)", Incorrect_Statement));
			return Code::Var_Create;
		}
		else if (it.value().second)
		{
			if (tokens.size() == 1)
				return Code::Var_Ret;
			if (tokens[1] != "=")
				return std::unexpected(ScriptError("unreckognized token, second token must be (=)", Incorrect_Statement));
			return Code::Var_Set;
		}
		else
		{
			if (tokens.size() == 1)
				return Code::Var_Ret;
			if (tokens.size() <= 2)
				return std::unexpected(ScriptError("Not enought to set a variable", Incomplete_Statement));
			if (tokens[1] != "at")
			{
				if (tokens[1] != "=")
					return std::unexpected(ScriptError("unreckognized token, second token must be (=) or (at)", Incorrect_Statement));
				return Code::Var_Set;
			}
			if (tokens.size() == 3)
				return Code::Var_Ret;
			if (tokens.size() == 4)
				return std::unexpected(ScriptError("Not enought to set a variable with at key word", Incomplete_Statement));
			if (tokens[3] != "=")
				return std::unexpected(ScriptError("unreckognized token, third token must be (=)", Incorrect_Statement));
			return Code::Var_Set;
		}
	}
}

std::pair<bool, int> Parser::str_to_int(const string& num) const
{
	try
	{
		return { true , std::stoi(num) };
	}
	catch (...)
	{
		return { false , {} };
	}
}

string to_lower(string word)
{
	std::for_each(word.begin(), word.end(), ::tolower);
	return word;
}

