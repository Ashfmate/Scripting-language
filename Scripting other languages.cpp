#include "ScriptEngine.h"
#include <conio.h>
#include <fmt\format.h>
#include <fmt\ranges.h>


int main()
{
	/*Engine eng;

	eng.run_engine("Scripting.txt");*/

	ScriptEngine eng;

	eng.set_var("double", 5.23423);
	eng.set_var("double", "Hello boi");
	eng.set_var("double", 1991);

	std::vector<std::string> res;

	auto him = eng.get_var_range("double");

	for (size_t i = 0; i < him.size(); ++i)
	{
		if (std::holds_alternative<bool>(him[i]))
			res.push_back(std::get<bool>(him[i]) ? "True" : "False");
		if (std::holds_alternative<char>(him[i]))
			res.push_back(std::string(std::get<char>(him[i]),1));
		if (std::holds_alternative<int>(him[i]))
			res.push_back(std::to_string(std::get<int>(him[i])));
		if (std::holds_alternative<double>(him[i]))
			res.push_back(std::to_string(std::get<double>(him[i])));
		if (std::holds_alternative<std::string>(him[i]))
			res.push_back(std::get<std::string>(him[i]));
	}

	fmt::print("The values of \"double\" are {}", res);

	while (!_kbhit());
	return 0;
}