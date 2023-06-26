#pragma once
#include <sstream>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>

#define parsing_check(line, word, error)					\
	do														\
	{														\
		if (!(line >> word))								\
			throw std::exception(error);					\
	} while(0);

#define var_const_input(word, var)							\
	do														\
	{														\
		if (is_num(word))									\
			set_memory(var, std::stod(word));				\
		else												\
			set_memory(var, get_memory(word));				\
	} while(0);

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
		key_words.emplace("iterate", [&](std::stringstream& line, std::string& word)
			{
				loop_index(line, word);
			});
	}

	// This function starts parsing the file to execute the script
	// @param path This is the path of the scripting file
	void run_engine(std::string path)
	{
		std::ifstream file(path);
		std::stringstream file_stream;
		std::stringstream line;
		file_stream << file.rdbuf();

		while (getline(file_stream, line))
		{
			std::string word;
			line >> word;
			if (auto it = key_words.find(word); it == key_words.end() || word == "exit")
				break;
			else
				it->second(line, word);
			line.clear();
		}
	}
#pragma region Private_Mapped_Functions
private:

	// Defines a variable
	void make_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote var and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid initialization, no variable name is provided to initialize anything to");
		// If the person wants to give an initial value
		if (!line.eof())
		{
			std::string var_name = word;
			line >> word;
			// If the person wants to initialize with a constant value
			if (is_num(word))
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
		parsing_check(line, word, "Invalid assignment, no variable is provided to assign anything to");
		std::string var_name = word;
		// If the person just wrote assign [var name] and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid assignment, no constant or existing variable is provided");
		// If the person entered a variable or a constant number
		var_const_input(word, var_name);
	}
	// Prints a variable or string with an new line
	void print_line_var(std::stringstream& line, std::string& word)
	{
		// If the person just wrote print and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid printing, no variable is provided for printing");
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
		parsing_check(line, word, "Invalid printing, no variable is provided for printing");
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
		parsing_check(line, word, "Invalid adding, no variable is provided for adding");
		// If the person enters a number or a variable
		var_const_input(word, first);
		// storing the value into second
		// If the person enters just add [var name OR constant] and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid adding, no second variable or constant is provided for adding");
		// If the person enters a number or a variable
		var_const_input(word, second);

		if (!(line >> word) && word != "into")
			throw std::exception("Invalid adding, the keyword into is not provided");
		parsing_check(line, word, "Invalid adding, a variable is not provided");
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
		parsing_check(line, word, "Invalid subracting, no variable is provided for subtracting");
		// If the person enters a number or a variable
		var_const_input(word, first);
		// storing the value into second
		// If the person enters just minus [var name OR constant] and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid subracting, no second variable or constant is provided for subtracting");
		// If the person enters a number or a variable
		var_const_input(word, second);

		// If the person enters just minus [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid subracting, the keyword into is not provided");
		// If the person enters just minus [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid subracting, a variable is not provided");
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
		parsing_check(line, word, "Invalid multiplication, no variable is provided for multiplying");
		// If the person enters a number or a variable
		var_const_input(word, first);
		// storing the value into second
		// If the person enters just multiply [var name OR constant] and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid multiplication, no second variable or constant is provided for multiplying");
		// If the person enters a number or a variable
		var_const_input(word, second);

		// If the person enters just multiply [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid multiplication, the keyword into is not provided");
		// If the person enters just multiply [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid multiplication, a variable is not provided");
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
		parsing_check(line, word, "Invalid division, no variable is provided for dividing");
		// If the person enters a number or a variable
		var_const_input(word, first);
		// storing the value into second
		// If the person enters just divide [var name OR constant] and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid division, no second variable or constant is provided for dividing");
		// If the person enters a number or a variable
		var_const_input(word, second);

		// If the person enters just divide [var name OR constants] [var name OR constants] and nothing else then throw, otherwise continue
		if (!(line >> word) && word != "into")
			throw std::exception("Invalid division, the keyword into is not provided");
		// If the person enters just divide [var name OR constants] [var name OR constants] into and nothing else then throw, otherwise continue
		parsing_check(line, word, "Invalid division, a variable is not provided");
		set_memory(word, first / second);
	}
	// This is a loop that loops per an index where an end is pronounced
	void loop_index(std::stringstream& line, std::string& word)
	{
		auto next_add = [](int& num)
		{
			++num;
		};
		auto next_sub = [](int& num)
		{
			--num;
		};
		void(*next)(int& num) = next_add;
		double start__ = 0.0;
		double end__ = 0.0;

		// If the person only entered "iterate" without start, end or a function
		parsing_check(line, word, "Invalid iteration, must provide the starting number");
		// If the person enters a number or a variable
		var_const_input(word, start__);
		// If the person only entered "iterator [start]" without end or a function
		parsing_check(line, word, "Invalid iteration, must provide the ending number");
		// If the person enters a number or a variable
		var_const_input(word, end__);
		// If the person only entered "iterator [start] [end]" without a function
		parsing_check(line, word, "Invalid iteration, must provide a statement to loop");
		auto it = key_words.find(word);
		if (it == key_words.end())
			throw std::exception("Invalid iteration, statement is unknown");

		// Round them into integers so that the for loop would be better
		int start = (int)round(start__);
		int end = (int)round(end__);

		// If the person specified two variables of the same size, then the it will only be done once
		if (start == end)
			it->second(line, word);
		// If the person made the end less than the start then it will be looped backwards
		else if (start > end)
			next = next_sub;

		std::stringstream line_copy;
		getline(line, line_copy);
		std::string line_copy_str = word + line_copy.str();
		// A try catch so that if the variable (#) is already made, this then stores the value in a temp variable then returns it back
		bool is_iterate_variable_allocated = false;
		try
		{
			allocate_memory("#");
		}
		catch (const std::exception&)
		{
			is_iterate_variable_allocated = true;
			set_memory("__temp", get_memory("#"));
		}
		// The loop of the function
		for (; start != end; next(start))
		{
			set_memory("#", double(start));
			line_copy.str(line_copy_str);
			std::string word_copy;
			line_copy >> word_copy;
			it->second(line_copy, word_copy);
			line_copy.clear();
		}
		// Returning the iterate variable back to its original value or deallocating it
		if (!is_iterate_variable_allocated)
			deallocate_memory("#");
		else
			set_memory("#", get_memory("__temp"));
	}

#pragma endregion

#pragma region Private_Helper_Functions

private:
	// A way to turn a line of std::cin or any other input stream into a stringstream
	// @param istream This is for the std::cin to be used, can also use any other input stream
	// @param stringstream This is used to store the std::cin line of contents
	// @return This is used to return the new state that the parameter istream is at
	std::istream& getline(std::istream& istream, std::stringstream& stringstream)
	{
		std::string line;
		for (char cur = istream.get(); istream.good(); cur = istream.get())
		{
			if (cur == '\n' || cur == ';')
				break;
			line.push_back(cur);
		}
		stringstream.str(line);
		return istream;
	}

	// Helper function to assign a value to piece in memory
	// @param name Used for the name of the variable
	// @param initial_val Used as the initial value of the variable
	const Engine& allocate_memory(const std::string name, double initial_val = 0.0)
	{
		if (auto it = variables.try_emplace(name,initial_val); it.second) return *this;
		
		throw std::exception("Invalid allocation, cannot duplicate allocation");
	}

	// Helper function to delete a variable in memory
	// @param name Used for the name of the variable to be deleted
	const Engine& deallocate_memory(const std::string name)
	{
		if (auto it = variables.find(name); it != variables.end())
			variables.erase(it);
		else
			throw std::exception("Invalid deallocation, cannot deallocate a non existing variable");
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

	// This function only exists for simplicity
	inline void set_memory(double& var, double val)
	{
		var = val;
	}

	// This helper function checks if a string is all numeric or not
	bool is_num(const std::string& str)
	{
		if (str.empty()) return false;

		bool has_decimal = false;
		size_t i = 0;

		if (str[0] == '-') i = 1;

		for (; i < str.size(); ++i)
		{
			if (std::isdigit(str[i])) continue;

			else if (str[i] != '.') return false;  // invalid character

			else if (has_decimal) return false;

			has_decimal = true;
		}

		return true;
	}

#pragma endregion

#pragma region Private_Variables

private:
	// Pointers used for the names of the variables
	std::unordered_map<std::string, double> variables;
	std::unordered_map<std::string, std::function<void(std::stringstream&, std::string&)>> key_words;

#pragma endregion
};

// TODO UNTIL I AM DONE WITH THIS PROJECT
// Make a conditional if statement
// Make it so that the code has structure
// Make it possible to write functions
// Make it possible for multiple datatypes to be declared