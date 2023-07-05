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
import Code;

using std::vector, std::string;

namespace
{
	vector<string> var_names;

	typedef std::pair<vector<string>, code::StatementType> CodeStatement;
	typedef vector<CodeStatement> CodeLine;
	typedef vector<CodeLine> CodeBlock;

	const err::Expect<CodeStatement> parseLine(const vector<string>& tokens, size_t& index);
	const err::Expect<std::pair<bool, std::optional<int>>> findVariable(string var);
	const err::Expect<code::Type> getType(const string& val);

}