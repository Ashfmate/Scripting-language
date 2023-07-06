export module Code;
import <optional>;
import <variant>;
import <string>;
import <vector>;
import <memory>;

using std::string, std::vector;

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
		Arg_Col,
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

	typedef std::pair<vector<string>, StatementType> CodeStatement;
	typedef vector<CodeStatement> CodeLine;
	typedef vector<CodeLine> CodeBlock;

	struct Code
	{
		std::shared_ptr<vector<DataType>> data_types;
		std::shared_ptr<vector<DataType>> ret_val;
		std::optional<string> name;
		StatementType type = code::StatementType::Empty_Statement;
		vector<Code> inner_code;
	};
}