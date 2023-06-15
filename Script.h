#pragma once
#include <sstream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <string>
#include <Windows.h>
#include <concepts>

template <typename A>
concept Action = requires {std::invocable<A>; };

class Function_Mapper
{
public:
	Function_Mapper() = default;
	Function_Mapper(const Function_Mapper& other)
	{
		mapper = other.mapper;
	}
	bool Change(const std::string& name, const Action auto func) const
	{
		if (auto it = mapper.find(name); it == mapper.end())
			return false;
		else
			it->second = func;
		return true;
	}
	bool Add(std::string name, const Action auto& func)
	{
		auto [it, inserted] = mapper.emplace(name, func);
		return inserted;
	}
	bool Add(std::string name)
	{
		auto [it, inserted] = mapper.emplace(name, [] { OutputDebugString(L"Default function"); });
		return inserted;
	}
	bool Remove(std::string name)
	{
		auto it = mapper.find(name);
		if (it == mapper.end())
			return false;
		mapper.erase(it);
		return true;
	}
	bool Call(std::string name)
	{
		auto it = mapper.find(name);
		if (it == mapper.end())
			return false;
		it->second();
		return true;
	}
private:
	std::unordered_map<std::string, std::function<void()>> mapper;
};