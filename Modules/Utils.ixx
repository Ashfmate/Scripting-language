export module Utils;
import <optional>;
import <string>;
import <algorithm>;

using std::string;

namespace utils
{
	export std::optional<int> toInt(std::string num)
	{
		try
		{
			return std::stoi(num);
		}
		catch (...)
		{
			return {};
		}
	}

	export bool is_num(const std::string& str)
	{
		if (str.empty()) return false;

		bool has_decimal = false;
		size_t i = 0;

		if (str[0] == '-') i = 1;

		for (; i < str.size(); ++i)
		{
			if (std::isdigit(str[i])) continue;

			else if (str[i] != '.') return false;  // invalid character

			else if (has_decimal) return false;

			has_decimal = true;
		}

		return true;
	}

	export std::string to_lower(std::string word)
	{
		std::for_each(word.begin(), word.end(), ::tolower);
		return word;
	}

	export const size_t estimateSize(const std::string& line, const std::string& delim)
	{
		return std::count_if(line.begin(), line.end(), [&delim](const char ch)
			{
				return (delim.find(ch) != std::string::npos);
			});
	}
}
