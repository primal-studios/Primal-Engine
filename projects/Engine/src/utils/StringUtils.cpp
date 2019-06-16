#include "utils/StringUtils.h"

#include <sstream>
#include <locale>
#include <algorithm>

std::vector<std::string> StringUtils::split(const std::string& aString, const char aDelimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(aString.c_str());
	while (std::getline(tokenStream, token, aDelimiter))
	{
		tokens.push_back(token.c_str());
	}

	return tokens;
}

std::vector<std::string> StringUtils::split(const std::string& aString, const std::string& aDelimiter)
{
	std::vector<std::string> tokens;

	size_t begin = 0;
	size_t next = aString.find(aDelimiter, begin);

	if (aString.length() == 0)
	{
		return tokens;
	}

	do
	{
		std::string val = aString.substr(begin, next - begin);

		if (next == begin || val.size() == 0)
		{
			begin = next + 1;
			next = aString.find(aDelimiter, begin + 1);
			continue;
		}

		if (std::string(aDelimiter).find(val[0]) != std::string::npos)
		{
			begin++;
			continue;
		}

		tokens.push_back(val);

		if (next >= aString.length())
		{
			break;
		}

		begin = next + 1;
		next = aString.find(aDelimiter, begin + 1);

	} while (begin < aString.length());

	return tokens;
}

std::vector<std::string> StringUtils::splitOnAny(const std::string& aString, const std::string& aDelimiters)
{
	std::vector<std::string> tokens;

	size_t begin = 0;
	size_t next = aString.find_first_of(aDelimiters, begin);

	if (aString.length() == 0)
	{
		return tokens;
	}

	do
	{
		std::string val = aString.substr(begin, next - begin);

		if (next == begin || val.size() == 0)
		{
			begin = next + 1;
			next = aString.find_first_of(aDelimiters, begin + 1);
			continue;
		}

		if (aDelimiters.find(val[0]) != std::string::npos)
		{
			begin++;
			continue;
		}

		tokens.push_back(val);

		if (next >= aString.length())
		{
			break;
		}

		begin = next + 1;
		next = aString.find_first_of(aDelimiters, begin + 1);

	} while (begin < aString.length());

	return tokens;
}

std::string StringUtils::trimFront(const std::string& aString, const uint32_t aAmount)
{
	return aString.substr(aAmount + 1);
}

std::string StringUtils::trimEnd(const std::string& aString, const uint32_t aAmount)
{
	return aString.substr(0, aString.length() - aAmount - 1);
}

std::string StringUtils::strip(const std::string& aString, const char aDelimiter)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (aString[i] != aDelimiter)
		{
			finalString += aString[i];
		}
	}

	return finalString;
}

std::string StringUtils::strip(const std::string& aString, const std::string& aDelimiter)
{
	std::string finalString = "";

	for (size_t index = 0; index < aString.length(); index++)
	{
		uint32_t matchCount = 0;

		for (size_t j = 0; j < aDelimiter.length(); j++)
		{
			if (aString[index + j] == aDelimiter[j])
			{
				matchCount++;
			}
		}

		const bool match = (matchCount == aDelimiter.length());

		if (match)
		{
			index += matchCount;
			continue;
		}

		finalString += aString[index];
	}

	return finalString;
}

std::string StringUtils::stripFirst(const std::string& aString, const char aDelimiter)
{
	std::string finalString = "";

	bool found = false;
	size_t index;
	for (index = 0; index < aString.length(); index++)
	{
		if (aString[index] != aDelimiter)
		{
			finalString += aString[index];
		}
		else
		{
			found = true;
			break;
		}
	}

	if (found)
	{
		finalString += aString.substr(index + 1);
	}

	return finalString;
}

std::string StringUtils::stripFirst(const std::string& aString, const std::string& aDelimiter)
{
	std::string finalString = "";

	bool found = false;
	size_t index;
	for (index = 0; index < aString.length(); index++)
	{
		uint32_t matchCount = 0;

		for (size_t j = 0; j < aDelimiter.length(); j++)
		{
			if (aString[index + j] == aDelimiter[j])
			{
				matchCount++;
			}
		}

		const bool match = (matchCount == aDelimiter.length());

		if (match)
		{
			index += matchCount;
			found = true;
			break;
		}

		finalString += aString[index];
	}

	if (found)
	{
		finalString += aString.substr(index + 1);
	}

	return finalString;
}

std::string StringUtils::stripLast(const std::string& aString, const char aDelimiter)
{
	return "";
}

std::string StringUtils::stripLast(const std::string& aString, const std::string& aDelimiter)
{
	return "";
}

std::string StringUtils::replace(const std::string& aString, const char aSearch, const char aReplacement)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (aString[i] == aSearch)
		{
			finalString += aReplacement;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replace(const std::string& aString, const std::string& aSearch, const char aReplacement)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		uint32_t matchCount = 0;

		for (size_t j = 0; j < aSearch.length(); j++)
		{
			if (aString[i + j] == aSearch[j])
			{
				matchCount++;
			}
		}

		if (matchCount == aSearch.length())
		{
			finalString += aReplacement;
			i += matchCount;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replace(const std::string& aString, const char aSearch, const std::string& aReplacement)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (aString[i] == aSearch)
		{
			finalString += aReplacement;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replace(const std::string& aString, const std::string& aSearch,
	const std::string& aReplacement)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		uint32_t matchCount = 0;

		for (size_t j = 0; j < aSearch.length(); j++)
		{
			if (aString[i + j] == aSearch[j])
			{
				matchCount++;
			}
		}

		if (matchCount == aSearch.length())
		{
			finalString += aReplacement;
			i += matchCount;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceFirst(const std::string& aString, const char aSearch, const char aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (aString[i] == aSearch && !replaced)
		{
			finalString += aReplacement;
			replaced = true;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceFirst(const std::string& aString, const std::string& aSearch, const char aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (size_t i = 0; i < aString.length(); i++)
	{
		uint32_t matchCount = 0;

		if (!replaced)
		{
			for (size_t j = 0; j < aSearch.length(); j++)
			{
				if (aString[i + j] == aSearch[j])
				{
					matchCount++;
				}
			}

			if (matchCount == aSearch.length())
			{
				finalString += aReplacement;
				i += matchCount;

				replaced = true;
				continue;
			}
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceFirst(const std::string& aString, const char aSearch, const std::string& aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (aString[i] == aSearch && !replaced)
		{
			finalString += aReplacement;
			replaced = true;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceFirst(const std::string& aString, const std::string& aSearch,
	const std::string& aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (size_t i = 0; i < aString.length(); i++)
	{
		uint32_t matchCount = 0;

		if (!replaced)
		{
			for (size_t j = 0; j < aSearch.length(); j++)
			{
				if (aString[i + j] == aSearch[j])
				{
					matchCount++;
				}
			}

			if (matchCount == aSearch.length())
			{
				finalString += aReplacement;
				i += matchCount;

				replaced = true;
				continue;
			}
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceLast(const std::string& aString, const char aSearch, const char aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (int32_t i = static_cast<int32_t>(aString.length()) - 1; i >= 0; i--)
	{
		if (aString[i] == aSearch && !replaced)
		{
			finalString += aReplacement;
			replaced = true;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceLast(const std::string& aString, const std::string& aSearch, const char aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (int32_t i = static_cast<int32_t>(aString.length()) - 1; i >= 0; i--)
	{
		uint32_t matchCount = 0;

		if (!replaced && (i - aSearch.length()) >= 0)
		{
			for (size_t j = 0; j < aSearch.length(); j++)
			{
				if (aString[i - j] == aSearch[aSearch.length() - 1 - j])
				{
					matchCount++;
				}
			}

			if (matchCount == aSearch.length())
			{
				finalString += aReplacement;
				i -= matchCount;

				replaced = true;
				continue;
			}
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceLast(const std::string& aString, const char aSearch, const std::string& aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (int32_t i = static_cast<int32_t>(aString.length()) - 1; i >= 0; i--)
	{
		if (aString[i] == aSearch && !replaced)
		{
			finalString += aReplacement;
			replaced = true;
			continue;
		}

		finalString += aString[i];
	}

	return finalString;
}

std::string StringUtils::replaceLast(const std::string& aString, const std::string& aSearch,
	const std::string& aReplacement)
{
	std::string finalString = "";
	bool replaced = false;

	for (int32_t i = static_cast<int32_t>(aString.length()) - 1; i >= 0; i--)
	{
		uint32_t matchCount = 0;

		if (!replaced && (i - aSearch.length()) >= 0)
		{
			for (size_t j = 0; j < aSearch.length(); j++)
			{
				if (aString[i - j] == aSearch[aSearch.length() - 1 - j])
				{
					matchCount++;
				}
			}

			if (matchCount == aSearch.length())
			{
				finalString += aReplacement;
				i -= matchCount;

				replaced = true;
				continue;
			}
		}

		finalString += aString[i];
	}

	finalString = reverse(finalString);

	return finalString;
}

std::string StringUtils::toLowercase(const std::string& aString)
{
	std::string str = aString.c_str();
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str.c_str();
}

std::string StringUtils::toUppercase(const std::string& aString)
{
	std::string str = aString.c_str();
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str.c_str();
}

std::string StringUtils::swapCase(const std::string& aString)
{
	std::string finalString = "";

	for (size_t i = 0; i < aString.length(); i++)
	{
		if (isupper(aString[i]))
		{
			finalString += std::to_string(tolower(aString[i]));
			continue;
		}

		if (islower(aString[i]))
		{
			finalString += std::to_string(toupper(aString[i]));
		}
	}

	return finalString;
}

bool StringUtils::equalsIgnoreCase(const std::string& aString, const std::string& aOther)
{
	const std::string a = toLowercase(aString);
	const std::string b = toLowercase(aOther);

	return a == b;
}

bool StringUtils::contains(const std::string& aString, const char aToContain)
{
	return aString.find(aToContain) != std::string::npos;
}

bool StringUtils::contains(const std::string& aString, const std::string& aToContain)
{
	return aString.find(aToContain) != std::string::npos;
}

bool StringUtils::containsAny(const std::string& aString, const std::vector<char>& aToContain)
{
	for (auto c : aToContain)
	{
		if (aString.find(c) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

bool StringUtils::containsAny(const std::string& aString, const std::vector<std::string>& aToContain)
{
	for (const auto& c : aToContain)
	{
		if (aString.find(c) != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

std::string StringUtils::deleteWhitespace(const std::string& aString)
{
	return strip(aString, "\t");
}

std::string StringUtils::reverse(const std::string& aString)
{
	std::string finalString = "";

	for (int32_t i = static_cast<int32_t>(aString.length()) - 1; i >= 0; i--)
	{
		finalString += aString[i];
	}

	return finalString;
}

bool StringUtils::startsWith(const std::string& aString, const char aPrefix)
{
	return aString[0] == aPrefix;
}

bool StringUtils::startsWith(const std::string& aString, const std::string& aPrefix)
{
	return aString.rfind(aPrefix, 0) == 0;
}

bool StringUtils::startsWithAny(const std::string& aString, const std::vector<char>& aPrefixes)
{
	for (const auto& p : aPrefixes)
	{
		if (startsWith(aString, p))
		{
			return true;
		}
	}

	return false;
}

bool StringUtils::startsWithAny(const std::string& aString, const std::vector<std::string>& aPrefixes)
{
	for (const auto& p : aPrefixes)
	{
		if (startsWith(aString, p))
		{
			return true;
		}
	}

	return false;
}

bool StringUtils::endsWith(const std::string& aString, const char aSuffix)
{
	return aString[aString.length() - 1] == aSuffix;
}

bool StringUtils::endsWith(const std::string& aString, const std::string& aSuffix)
{
	if (aSuffix.size() > aString.size()) return false;
	return (0 == aString.compare(aString.length() - aSuffix.length(), aSuffix.length(), aSuffix));
}

bool StringUtils::endsWithAny(const std::string& aString, const std::vector<char>& aSuffixes)
{
	for (const auto& c : aSuffixes)
	{
		if (endsWith(aString, c))
		{
			return true;
		}
	}

	return false;
}

bool StringUtils::endsWithAny(const std::string& aString, const std::vector<std::string>& aSuffixes)
{
	for (const auto& c : aSuffixes)
	{
		if (endsWith(aString, c))
		{
			return true;
		}
	}

	return false;
}