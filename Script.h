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

	// This function starts parsing an input from the console to execute the script
	void run_engine()
	{
		auto assign = [&](const double& val)
		{
			set_memory("num2", val);
		};

		std::stringstream line;
		getline(std::cin, line);
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
	// A way to turn a line of std::cin or any other input stream into a stringstream
	// @param istream This is for the std::cin to be used, can also use any other input stream
	// @param stringstream This is used to store the std::cin line of contents
	// @return This is used to return the new state that the parameter istream is at
	std::istream& getline(std::istream& istream, std::stringstream& stringstream)
	{
		std::string line;
		std::getline(istream, line);
		stringstream.str(line);
		return istream;
	}

	// Helper function to assign a value to piece in memory as well as making a pointer for it
	// @param name Used for the name of the variable
	// @param initial_val Used as the initial value of the variable
	const size_t allocate_memory(const std::string name, double initial_val = 0.0)
	{
		static size_t top = size_t(-1);
		if (++top == memory.size()) 
			throw std::exception("2 Allocation overflow");

		auto [it, inserted] = pointers.emplace(name, top);
		if (!inserted) 
		{
			top--;
			throw std::exception("3 Allocation duplicate, cannot duplicate allocation");
		}

		memory[top] = initial_val;
		return top;
	}

	// Helper function so that the look up is seemless
	// @param name Used for the look up of the variable
	// @return The variable in question
	inline const double get_memory(std::string name) const
	{
		if (auto it = pointers.find(name); it != pointers.end())
			return memory.at(it->second);
		throw std::exception(std::string("0 Bad memory get error; the supposed " + name + " does not exist").c_str());
	}

	// Helper function so that setting a variable is seemless
	// @param name Used for the look up of the variable
	// @param val Used for the new value of the variable
	inline void set_memory(const std::string& name, double val)
	{
		if (auto it = pointers.find(name); it != pointers.end())
			memory[it->second] = val;
		else
			throw std::exception(std::string("1 Bad memory set error; the supposed " + name + " does not exist").c_str());
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