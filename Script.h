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
	bool Change(const std::string& name, const Action auto func)
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
	Engine()
	{
		memory.resize(MaxStorageCapacity / sizeof(double));

		allocate_memory("num1");

		allocate_memory("num2");

		allocate_memory("is_value_given", -1.0);

		auto give = [&]
		{
			set_memory("is_value_given", 1.0);
		};

		auto add = [&]
		{
			set_memory("num1", get_memory("num1") + get_memory("num2"));
		};

		auto minus = [&]
		{
			set_memory("num1", get_memory("num1") - get_memory("num2"));
		};

		auto multiply = [&]
		{
			set_memory("num1", get_memory("num1") * get_memory("num2"));
		};

		auto divide = [&]
		{
			set_memory("num1", get_memory("num1") / get_memory("num2"));
		};

		auto print = [&]
		{
			std::cout << "\n\t\t\t---{" << get_memory("num1") << "}---\n";
		};

		func.Add("add", add);
		func.Add("subtract", minus);
		func.Add("multiply", multiply);
		func.Add("divide", divide);
		func.Add("print", print);
		func.Add("give", give);
		func.Add("assign");
	}

	// This function starts parsing the file to execute the script
	// @param path This is the path of the scripting file
	void run_engine(std::string path)
	{
		auto assign = [&](const double& val)
		{
			set_memory("num2", val);
		};

		std::ifstream file(path);
		std::stringstream line;
		line << file.rdbuf();
		std::string word;

		while (line >> word)
		{
			set_memory("is_value_given", -1.0);

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
	// Helper function to assign a value to piece in memory as well as making a pointer for it
	// @param name Used for the name of the variable
	// @param initial_val Used as the initial value of the variable
	// @return True if the number of variables are less than memory
	const bool allocate_memory(const std::string name, double initial_val = 0.0)
	{
		static size_t top = (size_t)-1;
		if (++top == memory.size()) return false;

		auto [it, inserted] = pointers.emplace(name, top);
		if (!inserted) top--;

		memory[std::distance(pointers.begin(),it)] = initial_val;

		return true;
	}
	// Helper function so that the look up is seemless
	// @param name Used for the look up of the variable
	// @return The variable in question
	inline const double get_memory(std::string name) const
	{
		return memory.at(pointers.at(name));
	}

	// Helper function so that setting a variable is seemless
	// @param name Used for the look up of the variable
	// @param val Used for the new value of the variable
	inline void set_memory(const std::string& name, double val)
	{
		memory[pointers[name]] = val;
	}
private:
	// A 32 Kilobyte storage capacity
	static constexpr size_t MaxStorageCapacity = (1 << 10) * 32;
	// Preallocates four thousand doubles in static memory
	std::vector<double> memory;
	// Pointers used for the names of the variables
	std::unordered_map<std::string, size_t> pointers;
	// The scripting container
	Function_Mapper func;
};