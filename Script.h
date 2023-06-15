#pragma once
#include <sstream>
#include <iostream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <string>
#include <Windows.h>
#include <concepts>
#include <array>

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

class Engine
{
public:
	Engine(Function_Mapper& func) : func(func)
	{
		allocate_memory("num1");

		allocate_memory("num2");

		allocate_memory("is_value_given", -1.0);

		auto give = [&]
		{
			memory[pointers["is_value_given"]] = 1.0;
		};

		auto add = [&]
		{
			get_memory("num1") += get_memory("num2");
		};

		auto minus = [&]
		{
			get_memory("num1") -= get_memory("num2");
		};

		auto multiply = [&]
		{
			get_memory("num1") *= get_memory("num2");
		};

		auto divide = [&]
		{
			get_memory("num1") /= get_memory("num2");
		};

		auto print = [&]
		{
			std::cout << "\n\t\t\t---{" << memory[pointers["num1"]] << "}---\n";
		};

		func.Add("add", add);
		func.Add("subtract", minus);
		func.Add("multiply", multiply);
		func.Add("divide", divide);
		func.Add("print", print);
		func.Add("give", give);
		func.Add("assign");
	}

	void run_engine(std::string path)
	{
		auto assign = [&](const double& val)
		{
			get_memory("num2") = val;
		};

		std::ifstream file(path);
		std::stringstream line;
		line << file.rdbuf();
		std::string word;

		while (line >> word)
		{
			get_memory("is_value_given") = -1.0;

			if (word == "exit" || !func.Call(word))
				break;
			if (get_memory("is_value_given") > 0.0)
			{
				line >> word;
				auto change = std::bind(assign, std::stoi(word));
				func.Change("assign", change);
				func.Call("assign");
			}
		}
	}
private:
	bool allocate_memory(std::string name, double initial_val = 0.0)
	{
		static size_t top;

		if (++top == memory.size()) return false;

		auto [it, inserted] = pointers.emplace(name, top);

		if (inserted) top--;

		memory[std::distance(pointers.begin(),it)] = initial_val;

		return true;
	}
	inline double& get_memory(std::string name)
	{
		return memory[pointers[name]];
	}
private:
	static constexpr size_t MaxStorageCapacity = (1 << 10) * 32;
	static std::array<double, MaxStorageCapacity / sizeof(double)> memory;
	static std::unordered_map<std::string, size_t> pointers;
	Function_Mapper func;
};