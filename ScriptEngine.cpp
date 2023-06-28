#include "ScriptEngine.h"

ScriptEngine::ScriptEngine(std::string path) : path(path) {}

std::expected<ScriptEngine*, script_error> ScriptEngine::set_var(std::string name, DataType value, size_t index)
{
	if (auto& it = variables[name]; index == 0)
		it.emplace_back(value);
	else if (index > it.size())
		return std::unexpected(script_error("Index out of range", script_error::Out_Of_Range));
	else
		it[index] = value;
	return this;
}

ScriptEngine& ScriptEngine::app_var(std::string name, DataType value)
{
	variables[name].push_back(value);
	return *this;
}

std::expected<DataType, script_error> ScriptEngine::get_var(std::string name, size_t index)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected(script_error("Variable does not exist", script_error::Var_Not_Exist));
	else
		return variables[name][index];
}

std::expected<std::vector<DataType>, script_error> ScriptEngine::get_var_range(std::string name)
{
	if (auto it = variables.find(name); it == variables.end())
		return std::unexpected{script_error("Variable does not exist", script_error::Var_Not_Exist)};
	else
		return variables[name];
}

ScriptEngine::Type ScriptEngine::get_type(DataType val)
{
	if (val)
		return Type(val.value().index());
	return Null;
}

void ScriptEngine::start()
{
	Parser(*this)(path);
}

ScriptEngine::Parser::Parser(ScriptEngine& eng) : eng(eng)
{
	code.keywords.emplace(std::vector<std::string>({ "if", "elf", "else" }));
}

void ScriptEngine::Parser::operator()(std::string path)
{
	std::ifstream file(path);
	std::string line;
	std::stringstream line_stream;
	while (std::getline(file, line, '\n'))
	{
		if (line == "exit")
			break;
		size_t assignment = line.find('=');
		if (assignment != std::string::npos)
		{
			line_stream << to_lower(line.substr(0, assignment));
			std::string var_name;
			line_stream >> var_name;
			line_stream << line.substr(assignment + 1);
			std::string cur_val;
			std::string quote_val;
			bool in_quote = false;
			while (line_stream >> cur_val)
			{
				if (cur_val == ",")
					continue;
				cur_val = to_lower(cur_val);
				in_quote = in_quote || cur_val.front() == '\"';
				if (in_quote)
				{
					quote_val += cur_val;
					if (cur_val.back() == '\"')
					{
						in_quote = false;
						cur_val = quote_val;
					}
					else
					{
						quote_val.push_back(' ');
						continue;
					}
				}
				if (cur_val.back() == ',')
					cur_val.pop_back();
				auto& val = parse_type(cur_val);
				if (val)
					eng.app_var(var_name, val.value());
			}
		}
		else break;
	}
}

const bool ScriptEngine::Parser::is_key_word(const std::string word) const
{
	return std::ranges::contains(*code.keywords, word);
}

const std::expected<DataType, script_error> ScriptEngine::Parser::parse_type(const std::string word) const
{
	if (auto wrd = to_lower(word); wrd == "null")
		return std::nullopt;
	else if (is_num(wrd))
		return (wrd.find_first_of('.') == std::string::npos) ? std::stoi(wrd) : std::stod(wrd);
	else if (wrd == "true" || wrd == "false")
		return wrd == "true";
	else if (wrd.front() == wrd.back() && wrd.front() == '\"')
		return wrd.substr(1, wrd.size() - 2);
	else
		return std::unexpected(script_error("The value given is not supported", script_error::Type_Not_Supported));
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

std::string to_lower(std::string word)
{
	std::for_each(word.begin(), word.end(), ::tolower);
	return word;
}
