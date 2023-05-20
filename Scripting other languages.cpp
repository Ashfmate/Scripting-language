#include <iostream>
#include "Script.h"
#include <conio.h>

std::istream& getline(std::istream& istream, std::stringstream& stringstream)
{
	std::string line;
	std::getline(istream, line);
	stringstream = std::stringstream(line);
	return istream;
}

int main()
{
	Function_Mapper func_map;
	
	double num1 = 0;
	double num2 = 0;

	bool is_value_given = false;

	auto give = [&is_value_given]
	{
		is_value_given = true;
	};

	auto assign = [&num2](const double val)
	{
		num2 = val;
	};

	auto addition = [&num1, &num2]
	{
		num1 += num2;
	};

	auto subtraction = [&num1, &num2]
	{
		num1 -= num2;
	};

	auto multiplication = [&num1, &num2]
	{
		num1 *= num2;
	};

	auto division = [&num1, &num2]
	{
		num1 /= num2;
	};

	auto print = [&num1]
	{
		std::cout << "\n\t\t\t---{" << num1 << "}---\n";
	};

	func_map.Add("add", addition);
	func_map.Add("subtract", subtraction);
	func_map.Add("multiply", multiplication);
	func_map.Add("divide", division);
	func_map.Add("print", print);
	func_map.Add("give", give);
	func_map.Add("assign");

	bool exit = false;

	std::stringstream line;
	getline(std::cin, line);
	std::string word;

	while (line >> word)
	{
		is_value_given = false;
		if (word == "exit" || !func_map.Call(word))
			break;
		if (is_value_given)
		{
			line >> word;
			auto change = std::bind(assign, std::stoi(word));
			func_map.Change("assign", change);
		}
	}

	while (!_kbhit());
	return 0;
}