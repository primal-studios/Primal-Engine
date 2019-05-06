#ifndef stringutils_h__
#define stringutils_h__

#include <cstdint>

#include <string>
#include <vector>

class StringUtils
{
	public:
		static std::vector<std::string> split(const std::string& aString, const char aDelimiter);
		static std::vector<std::string> split(const std::string& aString, const std::string& aDelimiter);
		static std::vector<std::string> splitOnAny(const std::string& aString, const std::string& aDelimiters);

		static std::string trimFront(const std::string& aString, const uint32_t aAmount);
		static std::string trimEnd(const std::string& aString, const uint32_t aAmount);

		static std::string strip(const std::string& aString, const char aDelimiter);
		static std::string strip(const std::string& aString, const std::string& aDelimiter);

		static std::string stripFirst(const std::string& aString, const char aDelimiter);
		static std::string stripFirst(const std::string& aString, const std::string& aDelimiter);

		static std::string stripLast(const std::string& aString, const char aDelimiter);
		static std::string stripLast(const std::string& aString, const std::string& aDelimiter);

		static std::string replace(const std::string& aString, const char aSearch, const char aReplacement);
		static std::string replace(const std::string& aString, const std::string& aSearch, const char aReplacement);
		static std::string replace(const std::string& aString, const char aSearch, const std::string& aReplacement);
		static std::string replace(const std::string& aString, const std::string& aSearch, const std::string& aReplacement);

		static std::string replaceFirst(const std::string& aString, const char aSearch, const char aReplacement);
		static std::string replaceFirst(const std::string& aString, const std::string& aSearch, const char aReplacement);
		static std::string replaceFirst(const std::string& aString, const char aSearch, const std::string& aReplacement);
		static std::string replaceFirst(const std::string& aString, const std::string& aSearch, const std::string& aReplacement);

		static std::string replaceLast(const std::string& aString, const char aSearch, const char aReplacement);
		static std::string replaceLast(const std::string& aString, const std::string& aSearch, const char aReplacement);
		static std::string replaceLast(const std::string& aString, const char aSearch, const std::string& aReplacement);
		static std::string replaceLast(const std::string& aString, const std::string& aSearch, const std::string& aReplacement);

		static std::string toLowercase(const std::string& aString);
		static std::string toUppercase(const std::string& aString);
		static std::string swapCase(const std::string& aString);

		static bool equalsIgnoreCase(const std::string& aString, const std::string& aOther);

		static bool contains(const std::string& aString, const char aToContain);
		static bool contains(const std::string& aString, const std::string& aToContain);

		static bool containsAny(const std::string& aString, const std::vector<char>& aToContain);
		static bool containsAny(const std::string& aString, const std::vector<std::string>& aToContain);

		static std::string deleteWhitespace(const std::string& aString);

		static std::string reverse(const std::string& aString);

		static bool startsWith(const std::string& aString, const char aPrefix);
		static bool startsWith(const std::string& aString, const std::string& aPrefix);
		static bool startsWithAny(const std::string& aString, const std::vector<char>& aPrefixes);
		static bool startsWithAny(const std::string& aString, const std::vector<std::string>& aPrefixes);

		static bool endsWith(const std::string& aString, const char aSuffix);
		static bool endsWith(const std::string& aString, const std::string& aSuffix);
		static bool endsWithAny(const std::string& aString, const std::vector<char>& aSuffixes);
		static bool endsWithAny(const std::string& aString, const std::vector<std::string>& aSuffixes);
};

#endif // stringutils_h__