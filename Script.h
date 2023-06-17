#pragma once
#include <sstream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <string>

class Engine
{
public:
	Engine()
	{
		allocate_memory("__temp");
		allocate_memory("true", 1.0);
		allocate_memory("false", -1.0);
		key_words.emplace("var", make_var);
	}

	// This function starts parsing the file to execute the script
	// @param path This is the path of the scripting file
	void run_engine(std::string path)
	{
		std::ifstream file(path);
		std::stringstream stream;
		stream << file.rdbuf();
		std::string word;

		while (stream >> word)
		{
			
		}
	}
private:
	void make_var(std::stringstream& line, std::string& word)
	{
		line >> word;
		if (!line.eof())
		{
			std::string var_name = word;
			line >> word;
			if (is_var_exist(word))
				allocate_memory(var_name, get_memory(word));

			else if (std::all_of(word.begin(), word.end(), [](const char elem) { return (elem >= '0' && elem <= '9'); }))
				allocate_memory(var_name, std::stoi(word));

			else
				throw std::exception("3 Invalid initialization, either use constant value or already used variables");
		}
		else
			allocate_memory(word);
	}

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

	// Helper function to assign a value to piece in memory
	// @param name Used for the name of the variable
	// @param initial_val Used as the initial value of the variable
	const Engine& allocate_memory(const std::string name, double initial_val = 0.0)
	{
		auto [it, inserted] = variables.emplace(name, initial_val);
		if (!inserted) 
			throw std::exception("2 Allocation duplicate, cannot duplicate allocation");

		return *this;
	}

	// Helper function so that the look up is seemless
	// @param name Used for the look up of the variable
	// @return The variable in question
	inline const double get_memory(std::string name) const
	{
		if (auto it = variables.find(name); it != variables.end())
			return it->second;
		throw std::exception(std::string("0 Bad memory get error; the supposed " + name + " does not exist").c_str());
	}

	// Helper function so that setting a variable is seemless
	// @param name Used for the look up of the variable
	// @param val Used for the new value of the variable
	inline void set_memory(const std::string& name, double val)
	{
		if (auto it = variables.find(name); it != variables.end())
			it->second = val;
		else
			throw std::exception(std::string("1 Bad memory set error; the supposed " + name + " does not exist").c_str());
	}

	inline const bool is_var_exist(const std::string& name) const
	{
		return variables.contains(name);
	}
private:
	// Pointers used for the names of the variables
	std::unordered_map<std::string, double> variables;
	std::unordered_map<std::string, void(std::stringstream&, std::string&)> key_words;
};