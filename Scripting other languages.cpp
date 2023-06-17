#include "Script.h"
#include <conio.h>

int main()
{
	Engine eng;

	eng.run_engine("Scripting.txt");

	while (!_kbhit());
	return 0;
}