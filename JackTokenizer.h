#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include "utilities.h"

using namespace std;

enum TokenType { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST };
enum KeywordType { CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, KT_NULL, THIS };

class token
{
public:

	string tokenData;
	TokenType tokenType;
};

class JackTokenizer
{
	fstream _inputFile;

	bool _hasMoreCommands = true;

	vector<token> _currentLineTokens;
	int _currentLineTokensIndex = -1;

	TokenType _currentTokenType;

	// internal methods

	bool _openBlockComment = false;
	string removeComments(string str)
	{
		// Remove in-line comments

		size_t found = str.find("//");

		if (found != string::npos)
		{
			return str.substr(0, found);
		}

		// Remove block comments

		found = str.find("/*");

		if (found != string::npos)
		{
			if (str.find("*/") == string::npos) _openBlockComment = true;
			return str.substr(0, found);
		}

		// Remove API block comments

		found = str.find("/**");

		if (found != string::npos)
		{
			if (str.find("*/") == string::npos) _openBlockComment = true;
			return str.substr(0, found);
		}

		found = str.find("*/");

		if (found != string::npos)
		{
			_openBlockComment = false;
			return str.substr(found + 2);		
		}
		
		if (_openBlockComment) return "";

		return str;
	}


	set<char> symbols = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'};
	set<string> keywords = {"class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return"};

	bool isIdentifier(string str)
	{
		if (!str.size()) return false;

		if (isdigit(str[0])) return false;

		for (char c : str)
		{
			if (!isalpha(c) && !isdigit(c) && c != '_') return false;
		}
		
		return true;
	}

	vector<token> _split(string str)
	{
		vector <token> tokens;
		string currentToken = "";
		// store charachters in a string until we see a (space or symbol or double quote)
		
		for (int i = 0; i < str.size(); i++)
		{
			if ((symbols.find(str[i]) != symbols.end()) || str[i] == ' ' || str[i] == '"')
			{

				if (currentToken.size())
				{
					token newToken;

					if (!(keywords.find(currentToken) == keywords.end())) // keyboard
					{
						newToken.tokenType = TokenType::KEYWORD;
					}
					else if (isNumber(currentToken)) // integerConstant
					{
						// note: some validation must be done here: 0 - 32767
						newToken.tokenType = TokenType::INT_CONST;
					}
					else if (isIdentifier(currentToken)) // identifier
					{
						newToken.tokenType = TokenType::IDENTIFIER;
					}
					else
					{
						throw ("token is not defined: " + currentToken);
					}

					newToken.tokenData = currentToken;
					tokens.push_back(newToken);
					currentToken = "";
				}

				if (symbols.find(str[i]) != symbols.end()) // symbol
				{
					string temp = "";
					temp += str[i];

					token newToken;
					newToken.tokenData = temp;
					newToken.tokenType = TokenType::SYMBOL;

					tokens.push_back(newToken); // ================= error
				}
				else if (str[i] == '"') // stringConstant
				{
					i++;
					string temp = "";

					while (str[i] != '"')
					{
						temp += str[i];
						i++;
					}

					token newToken;
					newToken.tokenData = temp;
					newToken.tokenType = TokenType::STRING_CONST;

					tokens.push_back(newToken);
				}

			}
			else
			{
				currentToken += str[i];
			}

		}

		return tokens;
	}

public:

	JackTokenizer(const string& FilePath)
	{
		_inputFile.open(FilePath, ios::in);
		advance();
	}

	~JackTokenizer()
	{
		if (_inputFile.is_open()) _inputFile.close();
	}

	bool hasMoreTokens()
	{
		return _hasMoreCommands;
	}

	void advance()
	{

		if (!hasMoreTokens()) throw "error: no more tokens";

		while (true)
		{
			if ((_currentLineTokens.size() > 0) && ((int)(_currentLineTokens.size() - 1) > _currentLineTokensIndex))
			{
				_currentLineTokensIndex++;
				_currentTokenType = _currentLineTokens[_currentLineTokensIndex].tokenType;
				break;
			}
			else
			{
				string _currentCommand;

				if (getline(_inputFile, _currentCommand))
				{

					// step 1. remove comments
					_currentCommand = removeComments(_currentCommand);
					if (_openBlockComment) _currentCommand = "";

					// step 2. remove spaces
					_currentCommand.erase(0, _currentCommand.find_first_not_of(" \t\r\n"));
					_currentCommand.erase(_currentCommand.find_last_not_of(" \t\r\n") + 1);

					if (_currentCommand == "") continue;

					// step 3. splite the line into tokens

					_currentLineTokens = _split(_currentCommand);

					_hasMoreCommands = true;
					_currentLineTokensIndex = -1;

					// set the first token
					advance();
				}
				else
				{
					_hasMoreCommands = false;
				}

				break;
			}
		}
	}

	TokenType tokenType()
	{
		return _currentTokenType;
	}

	string keywordValue()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::KEYWORD) throw "error: The current token is not a identifier";

		return _currentLineTokens[_currentLineTokensIndex].tokenData;
	}

	KeywordType keywordType()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::KEYWORD) throw "error: The current token is not a keyword";

		string temp = _currentLineTokens[_currentLineTokensIndex].tokenData;

		if (temp == "class")
		{
			return KeywordType::CLASS;
		}
		else if (temp == "constructor")
		{
			return KeywordType::CONSTRUCTOR;
		}
		else if (temp == "function")
		{
			return KeywordType::FUNCTION;
		}
		else if (temp == "method")
		{
			return KeywordType::METHOD;
		}
		else if (temp == "field")
		{
			return KeywordType::FIELD;
		}
		else if (temp == "static")
		{
			return KeywordType::STATIC;
		}
		else if (temp == "var")
		{
			return KeywordType::VAR;
		}
		else if (temp == "int")
		{
			return KeywordType::INT;
		}
		else if (temp == "char")
		{
			return KeywordType::CHAR;
		}
		else if (temp == "boolean")
		{
			return KeywordType::BOOLEAN;
		}
		else if (temp == "void")
		{
			return KeywordType::VOID;
		}
		else if (temp == "true")
		{
			return KeywordType::TRUE;
		}
		else if (temp == "false")
		{
			return KeywordType::FALSE;
		}
		else if (temp == "null")
		{
			return KeywordType::KT_NULL;
		}
		else if (temp == "this")
		{
			return KeywordType::THIS;
		}
		else if (temp == "let")
		{
			return KeywordType::LET;
		}
		else if (temp == "do")
		{
			return KeywordType::DO;
		}
		else if (temp == "if")
		{
			return KeywordType::IF;
		}
		else if (temp == "else")
		{
			return KeywordType::ELSE;
		}
		else if (temp == "while")
		{
			return KeywordType::WHILE;
		}
		else if (temp == "return")
		{
			return KeywordType::RETURN;
		}
		else
		{
			throw "Error: indefined keyword";
		}
	}

	char symbol()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::SYMBOL) throw "error: The current token is not a symbol";

		return _currentLineTokens[_currentLineTokensIndex].tokenData[0];
	}

	string identifier()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::IDENTIFIER) throw "error: The current token is not a identifier";

		return _currentLineTokens[_currentLineTokensIndex].tokenData;
	}

	int intVal()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::INT_CONST) throw "error: The current token is not a int cosntant";

		return stoi(_currentLineTokens[_currentLineTokensIndex].tokenData);
	}

	string stringVal()
	{
		if (_currentLineTokens[_currentLineTokensIndex].tokenType != TokenType::STRING_CONST) throw "error: The current token is not a string constant";

		return _currentLineTokens[_currentLineTokensIndex].tokenData;
	}

};

