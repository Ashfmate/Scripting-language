#include "ScriptEngine.h"

ScriptEngine::ScriptEngine(std::string path) : path(path) {}

std::expected<ScriptEngine*, ScriptError> ScriptEngine::set_var(const std::string name, const DataType value, const size_t index)
{
	if (auto& it = variables[name]; index == 0)
		it.emplace_back(value);
	else if (index > it.size())
		return std::unexpected(ScriptError("Index out of range", ScriptError::Out_Of_Range));
	else
		it[index] = value;
	return this;
}

std::expected<ScriptEngine*, ScriptError> ScriptEngine::set_var(const std::string name, const std::vector<DataType> value)
{
	variables[name] = value;
	return this;
}

ScriptEngine& ScriptEngine::app_var(const std::string name, const DataType value)
{
	variables[name].push_back(value);
	return *this;
}

ScriptEngine& ScriptEngine::app_var(const std::string name, const std::vector<DataType> value)
{
	variables[name].append_range(value);
	return *this;
}

const std::expected<DataType, ScriptError> ScriptEngine::get_var(const std::string name, const size_t index)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected(ScriptError("Variable does not exist", ScriptError::Var_Not_Exist));
	else
		return variables[name][index];
}

const std::expected<std::vector<DataType>, ScriptError> ScriptEngine::get_var_range(const std::string name)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected{ScriptError("Variable does not exist", ScriptError::Var_Not_Exist)};
	else
		return variables[name];
}

const ScriptEngine::Type ScriptEngine::get_type(const DataType val) const
{
	if (val)
		return Type(val.value().index());
	return Null;
}

ScriptError ScriptEngine::start()
{
	return Parser(*this)(path);
}

ScriptEngine::Parser::Parser(ScriptEngine& eng) : eng(eng)
{
	key_words.append_range(std::vector<std::string>({"if","elf","else","print","println"}));
}

ScriptError ScriptEngine::Parser::operator()(std::string path)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line, '\n'))
	{
		if (line.empty()) continue;
		else if (line == "exit") break;
		else if (auto& cur = pick_statment(line); cur)
		{
			code_lines.emplace_back(*cur.value());
		}
	}
	for (auto& cur : code_lines)
	{
		if (auto err = exec_code(cur); err.which() != ScriptError::No_Error)
			return err;
	}
}

const std::expected<std::unique_ptr<Code>, ScriptError> ScriptEngine::Parser::pick_statment(const std::string line)
{
	std::istringstream line_stream(line);
	std::string cur_word;
	line_stream >> cur_word;
	cur_word = to_lower(cur_word);
	if (is_key_word(cur_word))
	{
		if (cur_word == "print")
			return print(line_stream, false);
		else if (cur_word == "println")
			return print(line_stream, true);
	}
	else if (/*isdigit(cur_word.front()) && */std::ranges::all_of(cur_word, isalnum))
		return create_var(line_stream);
	else
		return std::unexpected(ScriptError(cur_word + " is not a valid statement", ScriptError::Invalid_Statement));
}

const std::expected<std::unique_ptr<Code>, ScriptError> ScriptEngine::Parser::create_var(std::istringstream& line)
{
	line.seekg(0, std::ios::beg);
	// The resulting code
	std::unique_ptr<Code> pCode = std::make_unique<Code>();
	// Getting the name of the variable
	std::string var_name;
	line >> var_name;
	pCode->name.emplace(var_name);
	// If the variable already exists, then set it to Var_Set which indicates setting, else Var_Create indicates initialization
	pCode->type = find_variable(pCode->name.value()) != code_lines.end() ? Code::Var_Set : Code::Var_Create;
	// Parses throught the values to assign
	std::string cur_val;
	if (line >> cur_val; cur_val != "=")
		return std::unexpected(ScriptError("There is no assign operator (=)", ScriptError::Assign_Operator_Missing));

	if (auto& ans = parse_values(line); !ans)
		return std::unexpected(ans.error());
	else
		pCode->data_types = ans.value();
	// Var_Create: name (name of the variable) data_types (the values to set to)
	// Var_Set: name (name of the variable) data_types (the values to set to)
	return std::move(pCode);
}

const std::expected<std::unique_ptr<Code>, ScriptError> ScriptEngine::Parser::print(std::istringstream& line, bool new_line)
{
	std::unique_ptr<Code> pCode = std::make_unique<Code>();
	pCode->type = Code::Print;
	if (new_line)
		pCode->type = Code::Println;
	if (auto& ans = parse_values(line); !ans)
		return std::unexpected(ans.error());
	else
		pCode->data_types = ans.value();
	// Print : data_types (the values to set to)
	return std::move(pCode);
}

const ScriptError ScriptEngine::Parser::exec_code(const Code& code)
{
	switch (code.type)
	{
	case Code::Var_Create:
	{
		if (!code.name)
			return ScriptError("The name of the Variable is not given", ScriptError::Code_Missing_Name);
		if (code.data_types.empty())
			return ScriptError("No value to assign to the variable is given", ScriptError::Code_Missing_Values);
		eng.set_var(code.name.value(), code.data_types);
	}
		break;
	case Code::Var_Set:
	{
		for (int i = 0; i < code.data_types.size(); ++i)
			eng.set_var(code.name.value(), code.data_types[i], i);
	}
		break;
	case Code::Print:
	{
		std::string out;
		bool given_val = false;
		for (auto& elem : code.data_types)
		{
			given_val = true;
			switch (eng.get_type(elem))
			{
			case ScriptEngine::Boolean:
				out += (std::get<bool>(elem.value()) ? " True ," : " False ,");
				break;
			case ScriptEngine::Int:
				out += " " + std::to_string(std::get<int>(elem.value())) + " ,";
				break;
			case ScriptEngine::Double:
				out += " " + std::to_string(std::get<double>(elem.value())) + " ,";
				break;
			case ScriptEngine::String:
				out += " \"" + std::get<std::string>(elem.value()) + "\" ,";
				break;
			case ScriptEngine::Null:
				out += " Null ,";
				break;
			default:
				break;
			}
		}
		if (given_val)
		{
			out.pop_back();
			out.front() = '[';
			out.back() = ']';
		}
		fmt::print("{}", out);
	}
		break;
	case Code::Println:
	{
		std::string out;
		bool given_val = false;
		for (auto& elem : code.data_types)
		{
			given_val = true;
			switch (eng.get_type(elem))
			{
			case ScriptEngine::Boolean:
				out += (std::get<bool>(elem.value()) ? " True ," : " False ,");
				break;
			case ScriptEngine::Int:
				out += " " + std::to_string(std::get<int>(elem.value())) + " ,";
				break;
			case ScriptEngine::Double:
				out += " " + std::to_string(std::get<double>(elem.value())) + " ,";
				break;
			case ScriptEngine::String:
				out += " \"" + std::get<std::string>(elem.value()) + "\" ,";
				break;
			case ScriptEngine::Null:
				out += " Null ,";
				break;
			default:
				break;
			}
		}
		if (given_val)
		{
			out.pop_back();
			out.front() = '[';
			out.back() = ']';
		}
		fmt::print("{}\n", out);
	}
		break;
	default:
		break;
	}
	return ScriptError("Everything fine, no need to worry", ScriptError::No_Error);
}

const std::expected<std::string, ScriptError> ScriptEngine::Parser::parse_quotes(std::string first_word, std::istringstream& line)
{
	if (first_word.back() == '\"') return first_word;
	first_word.push_back(' ');
	std::string str;
	while (true)
	{
		if (!(line >> str)) return std::unexpected(ScriptError("Could not find the other quote", ScriptError::Missing_Quote));
		first_word += str;
		if (str.back() == '\"') break;
		first_word.push_back(' ');
	}
	return first_word;
}

const bool ScriptEngine::Parser::is_key_word(const std::string word) const
{
	return std::ranges::contains(key_words, word);
}

const std::expected<DataType, ScriptError> ScriptEngine::Parser::parse_type(const std::string word) const
{
	if (auto wrd = to_lower(word); wrd == "null")
		return std::nullopt;
	else if (is_num(wrd))
	{
		if (wrd.find_first_of('.') == std::string::npos)
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
		return std::unexpected(ScriptError("The value given is not supported", ScriptError::Type_Not_Supported));
}

const bool ScriptEngine::Parser::is_num(const std::string& str) const
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

const std::expected<std::vector<DataType>, ScriptError> ScriptEngine::Parser::parse_values(std::istringstream& line)
{
	std::string cur_val;
	std::vector<DataType> var_col;
	while (line >> cur_val)
	{
		// If the user inputs an unnecessary comma
		if (cur_val == ",") continue;
		// If the user inputs a string
		if (cur_val.front() == '\"')
		{
			auto res = parse_quotes(cur_val, line);
			if (!res) return std::unexpected(res.error());
			cur_val = res.value();
		}
		// If the user inputs a comma right next to the value (also unnecessary)
		if (cur_val.back() == ',') cur_val.pop_back();
		// If the user inputs an existing variable
		if (auto it = find_variable(cur_val); it != code_lines.end())
		{
			var_col.append_range(it->data_types);
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

const std::vector<Code>::iterator ScriptEngine::Parser::find_variable(const std::string& name)
{
	return std::ranges::find_if(code_lines, [&name](const Code& code) { return code.name == name; });
}

std::string to_lower(std::string word)
{
	std::for_each(word.begin(), word.end(), ::tolower);
	return word;
}
