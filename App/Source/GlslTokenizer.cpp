#include "GlslTokenizer.h"

namespace Scape {
	char* substr(const char* arr, int begin, int len)
	{
		char* res = new char[len + 1];
		for (int i = 0; i < len; i++)
			res[i] = *(arr + begin + i);
		res[len] = 0;
		return res;
	}

	GlslTokenizer::Token GlslTokenizer::Next()
	{
		SeekNextNonWhiteSpace();

		Token token = { Eof,
			&_source[_cursor], // Grab beginning of token
			0,
			0,
			0 };

		// End of file
		if (_cursor >= _sourceLength) return token;

		// Find Keywords/Itendifiers
		if (IsValidSymbolStart(_source[_cursor]))
		{
			token.Type = Identifier;
			while (_cursor < _sourceLength && IsValidSymbolChar(_source[_cursor]))
			{
				_cursor++;
				token.TextLen++;
			}

			if (IsValidKeyWord(substr(_source, _cursor - token.TextLen, token.TextLen)))
				token.Type = Keyword;

			return token;
		}
		// Find numbers
		if (IsValidNumberStart(_source[_cursor]))
		{
			token.Type = Integer;
			while (_cursor < _sourceLength && IsValidNumberChar(_source[_cursor]))
			{
				if (!IsValidNumberStart(_source[_cursor]))
					token.Type = Floating;

				_cursor++;
				token.TextLen++;
			}
			return token;
		}
		// Find Operators
		if (IsValidOperatorChar(_source[_cursor]))
		{
			token.Type = Operator;
			_cursor++;
			token.TextLen = 1;
			return token;
		}

		return token;
	}

	void GlslTokenizer::SeekNextNonWhiteSpace()
	{
		while (_cursor < _sourceLength && isspace(_source[_cursor]))
		{
			_cursor++;
		}
	}

	bool GlslTokenizer::IsValidSymbolStart(char c)
	{
		return isalpha(c) || c == '_';
	}
	
	bool GlslTokenizer::IsValidSymbolChar(char c)
	{
		return isalnum(c) || c == '_';
	}

	bool GlslTokenizer::IsValidNumberStart(char c)
	{
		return isdigit(c);
	}

	bool GlslTokenizer::IsValidNumberChar(char c)
	{
		return isdigit(c) || VALID_NUMBER_CHARS.contains(c);
	}
	bool GlslTokenizer::IsValidOperatorChar(char c)
	{
		return VALID_OPERATOR_CHARS.contains(c);
	}
	bool GlslTokenizer::IsValidKeyWord(const char* c)
	{
		for (const char* str : VALID_KEYWORDS)
			if (strcmp(str, c) == 0) return true;
		return false;
	}
}
