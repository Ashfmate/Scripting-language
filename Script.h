#pragma once
#include <sstream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <string>
#include <Windows.h>

class Function_Mapper
{
public:
	Function_Mapper() = default;
	Function_Mapper(const Function_Mapper& other)
	{
		mapper = other.mapper;
	}
	bool Change(std::string name, std::function<void()> func)
	{
		if (!mapper.contains(name))
			return false;
		functions[mapper[name]] = func;
		return true;
	}
	bool Add(std::string name, std::function<void()> func)
	{
		if (mapper.contains(name))
			return false;
		mapper.insert({ name, functions.size()});
		functions.push_back(func);
		return true;
	}
	bool Remove(std::string name)
	{
		if (!mapper.contains(name))
			return false;
		size_t place = mapper[name];
		functions.erase(functions.begin() + place, functions.begin() + place + 1);
		mapper.erase(name);
		return true;
	}
	bool Call(std::string name)
	{
		if (!mapper.contains(name))
			return false;
		functions[mapper[name]]();
		return true;
	}
	bool Call_all()
	{
		if (functions.empty())
			return false;

		for (auto& func : functions)
			func();

		return true;
	}
private:
	std::unordered_map<std::string, size_t> mapper;
	std::vector<std::function<void()>> functions;
};