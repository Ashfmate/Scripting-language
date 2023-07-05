export module Error;

import <string>;
export import <expected>;

export namespace err
{
	enum class ErrorCode
	{
		No_Error,
		// Error codes for tokens that don't exist
		Var_Not_Exist,
		Assign_Not_Exist,
		Values_Not_Exist,
		// Error codes for either subtokens missing
		Missing_Quote,
		Missing_Right_Brace,
		// Error codes for values not matching
		Unsupported_Type,
		Non_Integral_Index,
		Non_Positive_Index,
		Out_Of_Range,
		// Error codes for Incorrect statements
		Incorrect_Statement, // TODO Could be broken down into simpler Error codes
		Indexing_Unknown_Var,
		Empty_Index_Brace,
		Empty_Statement,
		Var_Not_AlphaNumeric,
	};

	template<typename type>
	class Expect : public std::expected<type, ErrorCode>
	{
		using std::expected<type, ErrorCode>::expected;
	};
}