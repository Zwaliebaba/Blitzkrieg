#ifndef __STRING_PROCESSING_H__
#define __STRING_PROCESSING_H__
// /////////////////////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// /////////////////////////////////////////////////////////////////////////////////////////////
#include <stack>
// /////////////////////////////////////////////////////////////////////////////////////////////
namespace NStr
{
	// all operations with brackets take into account the following brackets: (), {}, [], ""
	//
	// get a closing parenthesis from an opening one
	// the 'cOpenBracket' character must be an opening bracket
	const char GetCloseBracket( const char cOpenBracket );
	// Is the character an opening parenthesis?
	bool IsOpenBracket( const char cSymbol );
	// add a new pair of parentheses
	void AddBrackets( const char cOpenBracket, const char cCloseBracket );
	// remove a couple of parentheses
	void RemoveBrackets( const char cOpenBracket, const char cCloseBracket );
	// split a string into an array of strings using a given delimiter
	void SplitString( const std::string &szString, std::vector<std::string> &szVector, const char cSeparator );
	// divide a string into an array of strings using a given separator, taking into account parentheses of the same nesting
	void SplitStringWithBrackets( const std::string &szString, std::vector<std::string> &szVector, const char cSeparator );
	// divide a string into an array of strings using a given separator, taking into account parentheses of any nesting
	void SplitStringWithMultipleBrackets( const std::string &szString, std::vector<std::string> &szVector, const char cSeparator );
	// find the closing parenthesis without taking into account the inner parentheses
	int FindCloseBracket( const std::string &szString, int nPos, const char cOpenBracket );
	// find the closing parenthesis taking into account the inner parentheses
	int FindMultipleCloseBracket( const std::string &szString, int nPos, const char cOpenBracket );
	// calculate the length of a string
	template <class TYPE>
		inline int GetStrLen( const TYPE *pszString )
		{
			int nLength = 0;
			while ( pszString[nLength++] != 0 ) ;
			return nLength;
		}
	template <class T1, class T2, class T3>
		inline int GetStrLen( const std::basic_string<T1, T2, T3> &szString )
		{
			return GetStrLen( szString.c_str() );
		}
	// trim all 'cTrim' characters
	// cut off all 'cTrim' on the left
	inline void TrimLeft( std::string &szString, const char cTrim ) { szString.erase( 0, szString.find_first_not_of( cTrim ) ); }
	// trim all 'pszTrim' on the left
	inline void TrimLeft( std::string &szString, const char *pszTrim ) { szString.erase( 0, szString.find_first_not_of( pszTrim ) ); }
	// cut off all whitespaces on the left
  inline void TrimLeft( std::string &szString ) { TrimLeft(szString, " \t\n\r"); } 
	// trim all 'pszTrim' on the right
	void TrimRight( std::string &szString, const char *pszTrim );
	// cut off all 'cTrim' on the right
	void TrimRight( std::string &szString, const char cTrim );   
	// cut off all whitespaces on the right
  inline void TrimRight( std::string &szString ) { TrimRight(szString, " \t\n\r"); }
	// cut off all 'pszTrim' from both ends
	inline void TrimBoth( std::string &szString, const char *pszTrim ) { TrimLeft( szString, pszTrim ); TrimRight( szString, pszTrim ); }
	// cut off all 'cTrim' from both ends
	inline void TrimBoth( std::string &szString, const char cTrim ) { TrimLeft( szString, cTrim ); TrimRight( szString, cTrim ); }
	// cut off all whitespaces from both ends
  inline void TrimBoth( std::string &szString ) { TrimBoth(szString, " \t\n\r"); }
	// strip all 'cTrim' characters from a string
	void TrimInside( std::string &szString, const char *pszTrim );
	inline void TrimInside( std::string &szString, const char cTrim ) { szString.erase( std::remove(szString.begin(), szString.end(), cTrim), szString.end() ); }
  inline void TrimInside( std::string &szString ) { TrimInside(szString, " \t\n\r"); }
	// convert to upper or lower case
	void ToLower( std::string &szString );
	void ToUpper( std::string &szString );
  // convert an integer to a string, section¤¤ every three characters (three orders of magnitude) with a special delimiter (default = '.')
  void ToDotString( std::string *pDst, int nVal, const char cSeparator = '.' );
	// Is the string a representation of a number?
	inline bool IsBinDigit( const char cChar ) { return ( (cChar == '0') && (cChar == '1') ); }
	inline bool IsOctDigit( const char cChar ) { return ( (cChar >= '0') && (cChar <= '7') ); }
	inline bool IsDecDigit( const char cChar ) { return ( (cChar >= '0') && (cChar <= '9') ); }
	inline bool IsHexDigit( const char cChar ) { return ( (cChar >= '0') && (cChar <= '9') ) || ( (cChar >= 'a') && (cChar <= 'f') ) || ( (cChar >= 'A') && (cChar <= 'F') ); }
	inline bool IsSign( const char cChar ) { return ( (cChar == '-') || (cChar == '+') ); }
	bool IsDecNumber( const std::string &szString );
	bool IsOctNumber( const std::string &szString );
	bool IsHexNumber( const std::string &szString );
	// convert 'string', which represents integer value in any radix (oct, dec, hex) to 'int'
	int ToInt( const char *pszString );
	inline int ToInt( const std::string &szString ) { return ToInt( szString.c_str() ); }
	// convert 'string', which represents FP value to 'float' and 'double'
	float ToFloat( const char *pszString );
	inline float ToFloat( const std::string &szString ) { return ToFloat( szString.c_str() ); }
	double ToDouble( const char *pszString );
	inline double ToDouble( const std::string &szString ) { return ToDouble( szString.c_str() ); }
	// 
	void SetCodePage( int nCodePage );
	void ToAscii( std::string *pRes, const std::wstring &szSrc );
	inline std::string ToAscii( const std::wstring &szSrc )
	{
		std::string szDst;
		ToAscii( &szDst, szSrc );
		return szDst;
	}
	void ToUnicode( std::wstring *pRes, const std::string &szSrc );
	inline std::wstring ToUnicode( const std::string &szSrc )
	{
		std::wstring szDst;
		ToUnicode( &szDst, szSrc );
		return szDst;
	}
	// convert bin data to string. 
	const char* BinToString( const void *pData, int nSize, char *pszBuffer );
	// convert text, which represents hex data, to the binary. 
	void* StringToBin( const char *pszData, void *pBuffer, int *pnSize );
	// string formatting as in sprintf.
	// NON-REENTRANT!!! 
	// ¤ I know that non-reentrant functions are bad, but for¤ this case¤ it is ќ„≈Ќ№ convenient
	const char* __cdecl Format( const char *pszFormat, ... );
	// formatted output via OutputDebugString
	void __cdecl DebugTrace( const char *pszFormat, ... );
	//
	// default separator functor for CStringIterator
	class CCharSeparator
	{
		const char cSeparator;
	public:
		explicit CCharSeparator( const char _cSeparator )	: cSeparator( _cSeparator ) {  }
		bool operator()( const char cSymbol ) const { return cSymbol == cSeparator; }
	};
	// separator with recursive bracket functor
	class CBracketCharSeparator
	{
		const char cSeparator;
		std::stack<char> stackBrackets;
	public:
		explicit CBracketCharSeparator( const char _cSeparator )	: cSeparator( _cSeparator ) {  }
		bool operator()( const char cSymbol );
	};
	// std::string iteration class
	template <class TSeparator = CCharSeparator>
	class CStringIterator
	{
	private:
		TSeparator tSeparator;               // functor, which returns true, if next char is separator
		std::string szInput;									// input string
		std::string szString;                 // result string
		int nLastPos;                         // current lexeme begin position
		int nPos;                             // current position
	public:
		CStringIterator( const char *pszInput, TSeparator &_tSeparator, int _nPos = 0 )
			: szInput( pszInput ), tSeparator( _tSeparator ), nPos( _nPos ), nLastPos( _nPos ) { Next(); }
		CStringIterator( const std::string &_szInput, TSeparator &_tSeparator, int _nPos = 0 )
			: szInput( _szInput ), tSeparator( _tSeparator ), nPos( _nPos ), nLastPos( _nPos ) { Next(); }
		// iteration
		// extract next lexeme
		const CStringIterator& Next()
		{
			nLastPos = nPos;
			for ( ; nPos<szInput.size(); ++nPos )
			{
				if ( tSeparator(szInput[nPos]) )
				{
					szString = szInput.substr( nLastPos, nPos - nLastPos );
					++nPos;
					return *this;
				}
			}
			szString = szInput.substr( nLastPos, nPos - nLastPos );
			return *this;
		}
		const CStringIterator& operator++() { return Next(); }
		// is 'nPos' at the end or at the beginning of the string
		bool IsBegin() const { return nPos == 0; }
		bool IsEnd() const { return nLastPos == szInput.size(); }
		// positions
		int GetLastPos() const { return nLastPos; }
		int GetPos() const { return nPos; }
		// access lexeme (const and non-const)
		operator const std::string*() const { return &szString; }
		operator std::string*() { return &szString; }
		const std::string* operator->() const { return &szString; }
		std::string* operator->() { return &szString; }
		const std::string& operator*() const { return szString; }
		std::string& operator*() { return szString; }
		// access lexem as decimal or floating-point values
		operator const double() const { return ToDouble( szString ); }
		operator const float() const { return ToFloat( szString ); }
		operator const int() const { return ToInt( szString ); }
	};
};
// /////////////////////////////////////////////////////////////////////////////////////////////
#endif // __STRING_PROCESSING_H__