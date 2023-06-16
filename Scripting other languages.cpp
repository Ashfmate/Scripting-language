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
	Engine eng;

	eng.run_engine("Scripting.txt");

	while (!_kbhit());
	return 0;
}