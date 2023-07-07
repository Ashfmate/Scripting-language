#include <iostream>
#include <ranges>
#include <algorithm>
export module ScriptEngine;
import <functional>;
import <unordered_map>;
import <memory>;
import Code;
import Parser;
import Utils;
import Error;

namespace
{
	typedef std::unique_ptr<code::Code> Code;
	typedef const vector<string>& Tokens;
}

export class ScriptEngine
{
public:
	// TODO change path to std::filesystem::path
	ScriptEngine(const string& path = "");
	err::ErrorCode runCode();
	err::ErrorCode compile();
	err::ErrorCode link();
	void execute();
	vector<code::DataType> getVarValues(const std::string_view var_name);
private:
	Code createVar(Tokens tokens);
	Code setVar(Tokens tokens);
	Code returnValues(Tokens tokens);
	Code print(Tokens tokens);
	Code println(Tokens tokens);
private:
	const code::Type getDataType(const code::DataType val) const;
	const code::Code getCode(const std::string_view var) const;
private:
	std::unordered_map<code::StatementType,std::function<Code(Tokens)>> func_mapper;
	vector<vector<Code>> compiled_code;
	string path;
};

ScriptEngine::ScriptEngine(const string& path) : path(path)
{
	func_mapper.emplace(code::StatementType::Var_Create	,	[&](Tokens tokens) { return createVar(tokens);		});
	func_mapper.emplace(code::StatementType::Var_Set	,	[&](Tokens tokens) { return setVar(tokens);			});
	func_mapper.emplace(code::StatementType::Print		,	[&](Tokens tokens) { return print(tokens);			});
	func_mapper.emplace(code::StatementType::Println	,	[&](Tokens tokens) { return println(tokens);		});
	func_mapper.emplace(code::StatementType::Arg_Col	,	[&](Tokens tokens) { return returnValues(tokens);	});
}

err::ErrorCode ScriptEngine::runCode()
{
	if (auto err = compile(); err != err::ErrorCode::No_Error)
		return err;
	else if (err = link(); err != err::ErrorCode::No_Error)
		return err;
	execute();
	return err::ErrorCode::No_Error;
}

err::ErrorCode ScriptEngine::compile()
{
	auto parsed_code = (path.empty()) ? parse::parseCode() : parse::parseCode(path);
	if (!parsed_code)
		return parsed_code.error();

	for (auto& line : parsed_code.value())
	{
		compiled_code.emplace_back(vector<Code>{});
		for (auto& statement : line)
		{
			compiled_code.back().emplace_back(std::move(func_mapper[statement.second](statement.first)));
		}
	}
	return err::ErrorCode::No_Error;
}

err::ErrorCode ScriptEngine::link()
{
	for (auto& line : compiled_code)
	{
		for (size_t i = 0; i < line.size(); ++i)
		{
			auto& cur = line[i];
			switch (cur->type)
			{
			case code::StatementType::Print:
			case code::StatementType::Println:
			{
				cur->data_types = std::make_shared<vector<code::DataType>>();
				cur->ret_val = cur->data_types;
				if (++i == line.size())
					return err::ErrorCode::Values_Not_Exist;

				for (; i < line.size(); ++i)
				{
					if (auto inner = line[i]->type;
						inner != code::StatementType::Print && inner != code::StatementType::Println)
					{
						cur->data_types->append_range(*line[i]->ret_val);
					}
				}
			}
			break;
			}
		}
	}
	return err::ErrorCode::No_Error;
}

void ScriptEngine::execute()
{
	for (auto& line : compiled_code)
	{
		for (auto& cur : line)
		{
			
			switch (cur->type)
			{
			case code::StatementType::Print:
			case code::StatementType::Println:
			{
				string out;
				bool given_val = false;
				for (auto& elem : *cur->data_types)
				{
					given_val = true;
					switch (getDataType(elem))
					{
					case code::Type::Boolean:
						out += (std::get<bool>(elem.value()) ? " True ," : " False ,");
						break;
					case code::Type::Int:
						out += " " + std::to_string(std::get<int>(elem.value())) + " ,";
						break;
					case code::Type::Double:
						out += " " + std::to_string(std::get<double>(elem.value())) + " ,";
						break;
					case code::Type::String:
						out += " \"" + std::get<std::string>(elem.value()) + "\" ,";
						break;
					case code::Type::Null:
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
				std::cout << out << "\n";
			}
			break;
			default:
				break;
			}
		}
	}
}

vector<code::DataType> ScriptEngine::getVarValues(const std::string_view var_name)
{
	for (auto& line : compiled_code)
		for (auto& execute : line)
			if (execute->name == var_name)
				return *execute->data_types;

	return vector<code::DataType>();
}

Code ScriptEngine::createVar(Tokens tokens)
{
	// Tokens will always be ([var_name] = [var_name] or "value")

	Code code_ptr = std::make_unique<code::Code>();
	code_ptr->name = tokens[0];
	code_ptr->data_types = std::make_shared<vector<code::DataType>>();
	for (size_t i = 2; i < tokens.size(); ++i)
	{
		if (auto var = parse::findVariable(tokens[i]).value(); var.first)
		{
			if (var.second)
				code_ptr->ret_val->push_back(getVarValues(tokens[i])[var.second.value()]);
			else
				code_ptr->ret_val->append_range(getVarValues(tokens[i]));
		}
		else
			code_ptr->data_types->emplace_back(parse::parseType(tokens[i]).value());
	}
	code_ptr->ret_val = code_ptr->data_types;
	code_ptr->type = code::StatementType::Var_Create;
	return std::move(code_ptr);
}
Code ScriptEngine::setVar(Tokens tokens)
{
	// Tokens will always be ([var_name] = [var_name] or "value")

	Code code_ptr = std::make_unique<code::Code>();

	auto& var_assign = parse::findVariable(tokens[0]).value();
	code_ptr->name = tokens[0].substr(0, tokens[0].find('['));

	code_ptr->data_types = getCode(tokens[0]).data_types;
	code_ptr->ret_val = std::make_shared<vector<code::DataType>>();
	for (size_t i = 2; i < tokens.size(); ++i)
	{
		if (auto var = parse::findVariable(tokens[i]).value(); var.first)
		{
			if (var.second)
				code_ptr->ret_val->emplace_back(getVarValues(tokens[i])[var.second.value()]);
			else
				code_ptr->ret_val->append_range(getVarValues(tokens[i]));
		}
		else
		{
			code_ptr->ret_val->emplace_back(parse::parseType(tokens[i]).value());
		}
	}

	for (size_t i = 0; i < code_ptr->ret_val->size(); ++i)
	{
		if (i >= code_ptr->data_types->size())
			code_ptr->data_types->emplace_back(code_ptr->ret_val->at(i));
		else
			code_ptr->data_types->at(i + var_assign.second.value_or(0)) = code_ptr->ret_val->at(i);
	}
	code_ptr->type = code::StatementType::Var_Set;
	return std::move(code_ptr);
}
Code ScriptEngine::returnValues(Tokens tokens)
{
	Code code_ptr = std::make_unique<code::Code>();
	code_ptr->data_types = std::make_shared<vector<code::DataType>>();
	code_ptr->ret_val = code_ptr->data_types;
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (auto var = parse::findVariable(tokens[i]).value(); var.first)
		{
			if (var.second)
				code_ptr->data_types->push_back(getVarValues(tokens[i])[var.second.value()]);
			else
				code_ptr->data_types->append_range(getVarValues(tokens[i]));
		}
		else
			code_ptr->data_types->emplace_back(parse::parseType(tokens[i]).value());
	}
	code_ptr->type = code::StatementType::Arg_Col;
	return std::move(code_ptr);
}
Code ScriptEngine::print(Tokens tokens)
{
	Code code_ptr = std::make_unique<code::Code>();
	code_ptr->name = tokens[0];
	code_ptr->type = code::StatementType::Print;
	return std::move(code_ptr);
}
Code ScriptEngine::println(Tokens tokens)
{
	Code code_ptr = std::make_unique<code::Code>();
	code_ptr->name = tokens[0];
	code_ptr->type = code::StatementType::Println;
	return std::move(code_ptr);
}

const code::Type ScriptEngine::getDataType(const code::DataType val) const
{
	if (!val)
		return code::Type::Null;
	switch (val.value().index())
	{
	case 0: return code::Type::Boolean;
		break;
	case 1: return code::Type::Int;
		break;
	case 2: return code::Type::Double;
		break;
	case 3: return code::Type::String;
		break;
	}
	return code::Type::Undefined;
}

const code::Code ScriptEngine::getCode(const std::string_view var) const
{
	auto name = var.substr(0, var.find('['));
	for (auto& line : compiled_code)
	{
		for (auto& execute : line)
		{
			if (execute->name == name)
				return *execute.get();
		}
	}
	return code::Code();
}
