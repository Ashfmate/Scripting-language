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

void ScriptEngine::start()
{
	Parser(*this)(path);
}

ScriptEngine::Parser::Parser(ScriptEngine& eng) : eng(eng)
{
	keywords.emplace(std::vector<std::string>({ "if", "elf", "else" }));
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
		pick_statment(line);
	}
}

const std::expected<void, ScriptError> ScriptEngine::Parser::pick_statment(const std::string line)
{
	std::istringstream line_stream(line);
	std::string cur_word;
	line_stream >> cur_word;
	line_stream.seekg(0, std::ios::beg);
	cur_word = to_lower(cur_word);
	if (auto it = find_key_word(line); it != keywords.value().end());
	else if (/*isdigit(cur_word.front()) && */std::ranges::all_of(cur_word, isalnum))
		create_var(line_stream);
	else
		return std::unexpected(ScriptError(cur_word + " is not a valid statement", ScriptError::Invalid_Statement));
}

const std::expected<void, ScriptError> ScriptEngine::Parser::create_var(std::istringstream& line)
{
	std::string var_name;
	line >> var_name;
	std::string cur_val;
	std::string quote_val;
	if (line >> cur_val; cur_val != "=")
		return std::unexpected(ScriptError("There is no assign operator (=)", ScriptError::Assign_Operator_Missing));
	std::vector<DataType> var_col;
	while (line >> cur_val)
	{
		if (cur_val == ",") continue;
		if (cur_val.front() == '\"')
		{
			auto res = parse_quotes(cur_val, line);
			if (!res) return std::unexpected(res.error());
			cur_val = res.value();
		}
		if (cur_val.back() == ',') cur_val.pop_back();
		auto& val = parse_type(to_lower(cur_val));
		if (!val) return std::unexpected(val.error());
		var_col.emplace_back(val.value());
	}
	eng.app_var(var_name, var_col);
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

const std::vector<std::string>::iterator ScriptEngine::Parser::find_key_word(const std::string word)
{
	return std::ranges::find(*keywords, word);
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

std::string to_lower(std::string word)
{
	std::for_each(word.begin(), word.end(), ::tolower);
	return word;
}
