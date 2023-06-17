#pragma once
#include <sstream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <iostream>

bool is_num(char c)
{
	return (c >= '0' && c <= '9') || c == '.';
}

class Engine
{
public:
	Engine()
	{
		allocate_memory("__temp");
		allocate_memory("true", 1.0);
		allocate_memory("false", -1.0);
		key_words.emplace("var", [&](std::stringstream& line, std::string& word)
			{
				make_var(line, word);
			});
		key_words.emplace("assign", [&](std::stringstream& line, std::string& word)
			{
				assign_var(line, word);
			});
		key_words.emplace("println", [&](std::stringstream& line, std::string& word)
			{
				print_line_var(line, word);
			});
		key_words.emplace("print", [&](std::stringstream& line, std::string& word)
			{
				print_var(line, word);
			});
		key_words.emplace("add", [&](std::stringstream& line, std::string& word)
			{
				add(line, word);
			});
		key_words.emplace("minus", [&](std::stringstream& line, std::string& word)
			{
				subract(line, word);
			});
		key_words.emplace("multiply", [&](std::stringstream& line, std::string& word)
			{
				multiply(line, word);
			});
		key_words.emplace("divide", [&](std::stringstream& line, std::string& word)
			{
				divide(line, word);
			});
	}

	// This function starts parsing the file to execute the script
	// @param path This is the path of the scripting file
	void run_engine(std::string path)
	{
		std::ifstream file(path);
		std::stringstream stream;

		while (!getline(file, stream).eof())
		{
			std::string word;
			stream >> word;
			if (auto it = key_words.find(word); it == key_words.end() || word == "exit")
				break;
			else
				it->second(stream, word);
			stream.clear();
		}
	}
private:
	// Defines a variable
	void make_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote var and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid initialization, no variable name is provided to initialize anything to");
		// If the person wants to give an initial value
		if (!line.eof())
		{
			std::string var_name = word;
			line >> word;
			// If the person wants to initialize with a constant value
			if (std::all_of(word.begin(), word.end(), is_num))
				allocate_memory(var_name, std::stod(word));
			// If the person wants to initialize with an existing variable
			else
				allocate_memory(var_name, get_memory(word));
		}
		// If the person just wants to allocate a variable with default initialization (0.0)
		else
			allocate_memory(word);
	}
	// Assigns to a variable
	void assign_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote assign and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid assignment, no variable is provided to assign anything to");
		std::string var_name = word;
		// If the person just wrote assign [var name] and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid assignment, no constant or existing variable is provided");
		// Alternatively, if the person provided a constant
		if (std::all_of(word.begin(), word.end(), is_num))
			set_memory(var_name, std::stod(word));
		// If the person provided an existing variable name
		else
			set_memory(var_name, get_memory(word));
	}
	// Prints a variable or string with an new line
	void print_line_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote print and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid printing, no variable is provided for printing");
		if (word.front() == '\"' && word.back() == '\"')
			std::cout << word << "\n";
		else
			std::cout << get_memory(word) << "\n";
		if (!line.eof())
			throw std::exception("Invalid printing, statements after variable or string should not be provided");
	}
	// Prints a variable or a string
	void print_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote print and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid printing, no variable is provided for printing");
		if (word.front() == '\"' && word.back() == '\"')
			std::cout << word;
		else
			std::cout << get_memory(word);
		if (!line.eof())
			throw std::exception("Invalid printing, statements after variable or string should not be provided");
	}
	// Adds two variables or two constants written as add [var name OR constant] [var name OR constant] into [var name]
	// Akin to [var name] = [var name OR constant] + [var name OR constant];	
	void add(std::stringstream& line, std::string& word)
	{
		double first = 0.0;
		double second = 0.0;
		// storing the value into first
		// If the person enters just add and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid adding, no variable is provided for adding");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			first = std::stod(word);
		// If the person enters a variable
		else
			first = get_memory(word);
		// storing the value into second
		// If the person enters just add [var name OR constant] and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid adding, no second variable or constant is provided for adding");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			second = std::stod(word);
		// If the person enters a variable
		else
			second = get_memory(word);

		if (!(line >> word) && word != "into")
			throw std::exception("Invalid adding, the keyword into is not provided");
		if (!(line >> word))
			throw std::exception("Invalid adding, a variable is not provided");
		set_memory(word, first + second);
	}
	// Subracts two variables or two constants written as minus [var name OR constant] [var name OR constant] into [var name]
	// Akin to [var name] = [var name OR constant] - [var name OR constant];	
	void subract(std::stringstream& line, std::string& word)
	{
		double first = 0.0;
		double second = 0.0;
		// storing the value into first
		// If the person enters just minus and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid subracting, no variable is provided for subtracting");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			first = std::stod(word);
		// If the person enters a variable
		else
			first = get_memory(word);
		// storing the value into second
		// If the person enters just minus [var name OR constant] and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid subracting, no second variable or constant is provided for subtracting");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			second = std::stod(word);
		// If the person enters a variable
		else
			second = get_memory(word);

		// If the person enters just minus [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid subracting, the keyword into is not provided");
		// If the person enters just minus [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid subracting, a variable is not provided");
		set_memory(word, first - second);
	}
	// Multiplies two variables or two constants written as multiply [var name OR constant] [var name OR constant] into [var name]
	// Akin to [var name] = [var name OR constant] * [var name OR constant];
	void multiply(std::stringstream& line, std::string& word)
	{
		double first = 0.0;
		double second = 0.0;
		// storing the value into first
		// If the person enters just multiply and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid multiplication, no variable is provided for multiplying");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			first = std::stod(word);
		// If the person enters a variable
		else
			first = get_memory(word);
		// storing the value into second
		// If the person enters just multiply [var name OR constant] and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid multiplication, no second variable or constant is provided for multiplying");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			second = std::stod(word);
		// If the person enters a variable
		else
			second = get_memory(word);

		// If the person enters just multiply [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid multiplication, the keyword into is not provided");
		// If the person enters just multiply [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid multiplication, a variable is not provided");
		set_memory(word, first * second);
	}
	// Divides two variables or two constants written as divide [var name OR constant] [var name OR constant] into [var name]
	// Akin to [var name] = [var name OR constant] / [var name OR constant];
	void divide(std::stringstream& line, std::string& word)
	{
		double first = 0.0;
		double second = 0.0;
		// storing the value into first
		// If the person enters just divide and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid division, no variable is provided for dividing");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			first = std::stod(word);
		// If the person enters a variable
		else
			first = get_memory(word);
		// storing the value into second
		// If the person enters just divide [var name OR constant] and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid division, no second variable or constant is provided for dividing");
		// If the person enters a number
		if (std::all_of(word.begin(), word.end(), is_num))
			second = std::stod(word);
		// If the person enters a variable
		else
			second = get_memory(word);

		// If the person enters just divide [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid division, the keyword into is not provided");
		// If the person enters just divide [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		if (!(line >> word))
			throw std::exception("Invalid division, a variable is not provided");
		set_memory(word, first / second);
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

	// Helper function to assign a value to piece in memory
	// @param name Used for the name of the variable
	// @param initial_val Used as the initial value of the variable
	const Engine& allocate_memory(const std::string name, double initial_val = 0.0)
	{
		auto [it, inserted] = variables.emplace(name, initial_val);
		if (!inserted) 
			throw std::exception("Allocation duplicate, cannot duplicate allocation");

		return *this;
	}

	// Helper function so that the look up is seemless
	// @param name Used for the look up of the variable
	// @return The variable in question
	inline const double get_memory(std::string name) const
	{
		if (auto it = variables.find(name); it != variables.end())
			return it->second;
		throw std::exception(std::string("Bad memory get error; the supposed " + name + " does not exist").c_str());
	}

	// Helper function so that setting a variable is seemless
	// @param name Used for the look up of the variable
	// @param val Used for the new value of the variable
	inline void set_memory(const std::string& name, double val)
	{
		if (auto it = variables.find(name); it != variables.end())
			it->second = val;
		else
			throw std::exception(std::string("Bad memory set error; the supposed " + name + " does not exist").c_str());
	}

	inline const bool is_var_exist(const std::string& name) const
	{
		return variables.contains(name);
	}
private:
	// Pointers used for the names of the variables
	std::unordered_map<std::string, double> variables;
	std::unordered_map<std::string, std::function<void(std::stringstream&, std::string&)>> key_words;
};