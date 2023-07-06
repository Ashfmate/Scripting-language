export module Utils;
import <optional>;
import <string>;
import <algorithm>;
import <vector>;

using std::string, std::vector;

export namespace utils
{
	enum NumType
	{
		None,
		Int,
		Double
	};
	/// <summary>
	/// checks if the string is an int or a double or neither (not number)
	/// </summary>
	/// <param name="str"> The string in question </param>
	/// <returns> 
	/// An enum, zero indicates an not number, one indicates Int and two indicates double
	/// </returns>
	NumType isNum(const std::string& str)
	{
		if (str.empty()) return None;

		bool has_decimal = false;
		size_t i = 0;

		if (str[0] == '-') i = 1;

		for (; i < str.size(); ++i)
		{
			if (std::isdigit(str[i])) continue;

			else if (str[i] != '.') return None;  // invalid character

			else if (has_decimal) return None;

			has_decimal = true;
		}

		return has_decimal? Double : Int;
	}
	/// <summary>
	/// Turns a string into an int, checked by isNum
	/// </summary>
	/// <param name="num"> The string of possible numbers</param>
	/// <returns> An int if it succeeded otherwise a nullopt </returns>
	const std::optional<int>& getNumType(const std::string& num)
	{
		if (isNum(num) == Int)
			return std::stoi(num);
		return std::nullopt;
	}
	/// <returns> Returns a lowercase version of word </returns>
	std::string toLower(const std::string& word)
	{
		std::for_each(word.begin(), word.end(), ::tolower);
		return word;
	}
	/// <summary>
	/// Estimates the size of the array of lines if it where to be split by the delim
	/// </summary>
	/// <param name="line"> The string to be split </param>
	/// <param name="delim"> Used to tell which character the "line" would split from </param>
	/// <returns> The size of the estimated array of split strings </returns>
	const size_t estimateSize(const std::string_view line, const std::string& delim)
	{
		return std::count_if(line.begin(), line.end(), [&delim](const char ch)
			{
				return (delim.find(ch) != std::string::npos);
			});
	}
	/// <summary>
	/// Moves a token into a vector if it has a value
	/// </summary>
	/// <param name="vec"> The vector of strings </param>
	/// <param name="token"> The string to be moved into the vector</param>
	/// <returns> Whether the token was moved or not</returns>
	bool moveToken(vector<string>& vec, string& token)
	{
		if (!token.empty())
		{
			token = toLower(token);
			vec.emplace_back(std::move(token));
			return true;
		}
		return false;
	}
}
