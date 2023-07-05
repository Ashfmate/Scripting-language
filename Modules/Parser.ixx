export module Parser;

import <memory>;
import <optional>;
import <vector>;
import <variant>;
import <ranges>;
import <fstream>;
import <sstream>;
import <vector>;
import <string>;
import Error;
import Utils;

using std::vector, std::string;


export namespace code
{
	using DataType = std::optional<std::variant<bool, int, double, string>>;

	enum class Type
		{
			Boolean,
			Int,
			Double,
			String,
			Null,
			Undefined,
			Count
		};

	enum class StatementType
		{
			Empty_Statement,
			Var_Ret,
			Var_Create,
			Var_Set,
			If,
			Else_If,
			Else,
			While,
			Iterate,
			Print,
			Println,
			Function
		};
	
	struct Code
	{
		std::shared_ptr<vector<DataType>> data_types;
		std::shared_ptr<vector<DataType>> ret_val;
		std::optional<string> name;
		StatementType type = code::StatementType::Empty_Statement;
		vector<Code> inner_code;
	};
}

namespace code
{
	vector<string> var_names;
}

namespace
{
	typedef std::pair<vector<string>, code::StatementType> CodeStatement;
	typedef vector<CodeStatement> CodeLine;
	typedef vector<CodeLine> CodeBlock;
}