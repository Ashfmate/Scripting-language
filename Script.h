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
		mapper[name] = func;
		return true;
	}
	bool Add(std::string name, std::function<void()> func)
	{
		if (mapper.contains(name))
			return false;
		mapper.insert({ name, func });
		return true;
	}
	bool Remove(std::string name)
	{
		if (!mapper.contains(name))
			return false;
		mapper.erase(name);
		return true;
	}
	bool Call(std::string name)
	{
		if (!mapper.contains(name))
			return false;
		mapper[name]();
		return true;
	}
private:
	std::unordered_map<std::string, std::function<void()>> mapper;
};