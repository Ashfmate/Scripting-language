#include "ScriptEngine.h"
#include <fmt\format.h>
#include <fmt\ranges.h>

int main()
{
	ScriptEngine eng("Scripting.txt");
	eng.start();

	system("pause");
	return 0;
}