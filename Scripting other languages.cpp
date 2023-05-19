#include <iostream>
#include "Script.h"
#include <conio.h>

bool foo()
{
	std::cout << "\n\n\nHello world (but cool)!";
	return false;
}

bool boo()
{
	std::cout << "\n\n\nHello world (but not that cool)!";
	return false;
}

bool goo()
{
	std::cout << "\n\n\nHello world (but it is alright)!";
	return false;
}

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
	func_map.Add("testA", foo);
	func_map.Add("testB", boo);
	func_map.Add("testC", goo);
	bool exit = false;

	std::list<std::function<void()>> list;
	std::stringstream line;
	getline(std::cin, line);
	std::string word;

	while (line >> word)
		if (word == "exit" || !func_map.Call(word))
			break;

	while (!_kbhit());
	return 0;
}