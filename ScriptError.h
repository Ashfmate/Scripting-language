#pragma once
#include <string>

enum Errors : int
{
	No_Error,
	Var_Not_Exist,
	Out_Of_Range,
	Type_Not_Supported,
	Incorrect_Statement,
	Invalid_Statement,
	Right_Square_Bracket_Missing,
	Index_Not_Integral,
	Incomplete_Statement,
	Assign_Operator_Missing,
	Missing_Quote,
	Code_Missing_Name,
	Code_Missing_Values
};
// Class for holding all error states and for passing the error message and their code
class ScriptError
{
public:
	ScriptError() = default;
	ScriptError(std::string msg, Errors err_code) : err_msg(msg), err_code(err_code) {}
	const int which() const
	{
		return err_code;
	}
	const std::string what() const
	{
		return err_msg;
	}
private:
	Errors err_code;
	std::string err_msg;
};