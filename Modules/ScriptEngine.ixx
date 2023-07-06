#include <iostream>
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
	Code createVar(Tokens tokens)
	{
		return nullptr;
	}
	Code setVar(Tokens tokens)
	{
		return nullptr;
	}
	Code retVal(Tokens tokens)
	{
		return nullptr;
	}
	Code retVar(Tokens tokens)
	{
		return nullptr;
	}
	Code print(Tokens tokens)
	{
		return nullptr;
	}
	Code println(Tokens tokens)
	{
		return nullptr;
	}
}

export class ScriptEngine
{
public:
	// TODO change path to std::filesystem::path
	ScriptEngine(const string& path = "");
	void compile();
	void execute();
private:
	
private:
	std::unordered_map<
		code::StatementType,std::function<std::unique_ptr<code::Code>(const vector<string>&)>> func_mapper;
	vector<std::unique_ptr<code::Code>> compiled_code;
	string path;
};

ScriptEngine::ScriptEngine(const string& path) : path(path)
{
	func_mapper.emplace(code::StatementType::Var_Create	,	createVar	);
	func_mapper.emplace(code::StatementType::Var_Set	,	setVar		);
	func_mapper.emplace(code::StatementType::Var_Ret	,	retVar		);
	func_mapper.emplace(code::StatementType::Print		,	print		);
	func_mapper.emplace(code::StatementType::Println	,	println		);
	func_mapper.emplace(code::StatementType::Arg_Col	,	retVal		);
}

void ScriptEngine::compile()
{
	auto parsed_code = (path.empty()) ? parse::parseCode() : parse::parseCode(path);
	if (!parsed_code)
	{
		std::cout << "Error code : " << (int)parsed_code.error() << " occured, fix it boi\n";
		return;
	}

	for (auto& line : parsed_code.value())
	{
		for (auto& statement : line)
		{
			compiled_code.emplace_back(std::move(func_mapper[statement.second](statement.first)));
		}
	}
}

void ScriptEngine::execute()
{

}
