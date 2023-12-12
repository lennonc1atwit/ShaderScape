#pragma once
#include <string>
#include <vector>
#include <set>
 
namespace Scape 
{
	class GlslTokenizer
	{
	public:
		enum Type
		{
			Invalid = -1,
			Eof = 0,
			Keyword,
			Identifier,
			Number,
			Integer,
			Floating,
			Operator,
			Syntax
		};

		typedef struct
		{
			Type Type;
			const char* Text;
			size_t TextLen;
			int Position;
			int Row;
			int Col;
		} Token;

		GlslTokenizer(const char* source, size_t length) : _source(source), _sourceLength(length) 
		{
			_cursor = 0;
			_line = 0;
			_lineStart = 0;
		};
		~GlslTokenizer() {};

		Token Next();
	private:
		void SeekNextNonWhiteSpace();
		bool IsValidSymbolStart(char c);
		bool IsValidSymbolChar(char c);
		bool IsValidNumberStart(char c);
		bool IsValidNumberChar(char c);
		bool IsValidOperatorChar(char c);
		bool IsValidKeyWord(const char* c);

		std::set<char> VALID_NUMBER_CHARS   = { '-',  '.',  'f',  'x',  'e' };
		std::set<char> VALID_OPERATOR_CHARS = { '-',  '+',  '*',  '/',  '=', '!', '>', '<', '[', ']',  '(', ')', ',', '.'};
		std::set<char> VALID_SYNTAX_CHARS = { '{', '}', ';' };
		std::set<const char*> VALID_KEYWORDS = { "const", "uniform", "buffer", "shared", "attribute", "varying" };

		const char* _source;
		size_t _sourceLength;
		size_t _cursor;
		size_t _lineStart;
		size_t _line;
	};
}