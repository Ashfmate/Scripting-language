#include "ScriptEngine.h"

#include "Parser.h"

const ScriptEngine::Type ScriptEngine::get_type(const DataType val) const
{
	if (val)
		return Type(val.value().index());
	return Null;
}

ScriptError ScriptEngine::start(string path)
{
	if (auto res = Parser(*this)(path))
	{
		code_lines.append_range(std::move(res.value()));
		return ScriptError("Everything working as intended", No_Error);
	}
	else
		return res.error();
}

ScriptError ScriptEngine::exec()
{
	for (auto& code : code_lines)
	{
		switch (code.type)
		{
		case Code::Print:
		case Code::Println:
		{
			string out;
			bool given_val = false;
			for (auto& elem : *code.data_types.get())
			{
				given_val = true;
				switch (get_type(elem))
				{
				case ScriptEngine::Boolean:
					out += (std::get<bool>(elem.value()) ? " True ," : " False ,");
					break;
				case ScriptEngine::Int:
					out += " " + std::to_string(std::get<int>(elem.value())) + " ,";
					break;
				case ScriptEngine::Double:
					out += " " + std::to_string(std::get<double>(elem.value())) + " ,";
					break;
				case ScriptEngine::String:
					out += " \"" + std::get<string>(elem.value()) + "\" ,";
					break;
				case ScriptEngine::Null:
					out += " Null ,";
					break;
				default:
					break;
				}
			}
			if (given_val)
			{
				out.pop_back();
				out.front() = '[';
				out.back() = ']';
			}
			fmt::print("{}{}", out, (code.type == Code::Println ? "\n" : ""));
		}
		break;
		}
	}
	return ScriptError("Everything fine, no need to worry", No_Error);
}