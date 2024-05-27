#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "JackTokenizer.h"

class CompilationEngine
{

	JackTokenizer* _jackTokenizer = nullptr;
	fstream _outputFile;

	template <typename Tdata> bool _checkExpectedToken(Tdata currentToken, Tdata expectedToken)
	{
		if (currentToken == expectedToken) return true;

		return false;
	}

	bool _eat(string token)
	{
		switch (_jackTokenizer->tokenType())
		{
		case KEYWORD:

			if (!_checkExpectedToken<string>(_jackTokenizer->keywordValue(), token)) return false;

			_outputFile << "<keyword> " + _jackTokenizer->keywordValue() + " </keyword>" << endl;

			break;
		case SYMBOL:

			if (_jackTokenizer->symbol() == '<')
			{
				if (!_checkExpectedToken<char>('<', token[0])) return false;
				_outputFile << "<symbol> &lt; </symbol>" << endl;
			}
			else if (_jackTokenizer->symbol() == '>')
			{
				if (!_checkExpectedToken<char>('>', token[0])) return false;
				_outputFile << "<symbol> &gt; </symbol>" << endl;
			}
			else if (_jackTokenizer->symbol() == '"')
			{
				if (!_checkExpectedToken<char>('"', token[0])) return false;
				_outputFile << "<symbol> &quot; </symbol>" << endl;
			}
			else if (_jackTokenizer->symbol() == '&')
			{
				if (!_checkExpectedToken<char>('&', token[0])) return false;
				_outputFile << "<symbol> &amp; </symbol>" << endl;
			}
			else
			{
				if (!_checkExpectedToken<char>(_jackTokenizer->symbol(), token[0])) return false;
				_outputFile << "<symbol> " << _jackTokenizer->symbol() << " </symbol>" << endl;
			}

			break;		
		default:

			return false;
		}

		_jackTokenizer->advance();
		return true;
	}

public:

	CompilationEngine(JackTokenizer* inputFile, string outputFile)
	{
		this->_jackTokenizer = inputFile;
		this->_outputFile.open(outputFile, ios::out);

		if (_jackTokenizer == nullptr) throw "Error : JackTokenizer is NULL.";

		CompileClass();
	}

	~CompilationEngine()
	{
		delete _jackTokenizer;
		if (_outputFile.is_open()) _outputFile.close();
	}

	// Program structure

	void CompileClass()
	{
		_outputFile << "<class>" << endl; // open class tage

		if (!_eat("class")) throw "Error keyword: Current token does not equal expected token.";

		_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
		_jackTokenizer->advance();

		if (!_eat("{")) throw "Error symbol: Current token does not equal expected token.";

		while(_jackTokenizer->tokenType() == TokenType::KEYWORD && (_jackTokenizer->keywordType() == KeywordType::STATIC || _jackTokenizer->keywordType() == KeywordType::FIELD))
		{
			CompileClassVarDec();// 0 or more
		}
		
		while (_jackTokenizer->tokenType() == TokenType::KEYWORD && (_jackTokenizer->keywordType() == KeywordType::CONSTRUCTOR || _jackTokenizer->keywordType() == KeywordType::FUNCTION || _jackTokenizer->keywordType() == KeywordType::METHOD))
		{
			CompileSubroutineDec(); // 0 or more
		}

		if (!_eat("}")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</class>" << endl; // close class tage
	}

	void CompileClassVarDec()
	{
		_outputFile << "<classVarDec>" << endl; // open classVarDec tage

		if (!(_eat("static") || _eat("field"))) throw "Error keyword: Current token does not equal expected token.";

		// type
		if (_jackTokenizer->tokenType() == TokenType::KEYWORD)
		{
			if (_jackTokenizer->keywordType() == KeywordType::INT)
			{
				_eat("int");
			}
			else if (_jackTokenizer->keywordType() == KeywordType::CHAR)
			{
				_eat("char");
			}
			else if (_jackTokenizer->keywordType() == KeywordType::BOOLEAN)
			{
				_eat("boolean");
			}
			else
			{
				throw "Error Type: this Data type is not vaild keyword.";
			}
		}
		else if (_jackTokenizer->tokenType() == TokenType::IDENTIFIER)
		{
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();
		}
		else
		{
			throw "Error Type: this Data type is not valid.";
		}

		// varName 1 or more
		while(true)
		{
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();

			if (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ',')) break;

			_eat(",");
		}
		
		if (!_eat(";")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</classVarDec>" << endl; // close classVarDec tage
	}

	void CompileSubroutineDec()
	{
		_outputFile << "<subroutineDec>" << endl; // open subroutineDec tage

		if (!(_eat("constructor") || _eat("function") || _eat("method"))) throw "Error keyword: Current token does not equal expected token.";

		// type
		if (_jackTokenizer->tokenType() == TokenType::KEYWORD)
		{
			if (_jackTokenizer->keywordType() == KeywordType::VOID)
			{
				_eat("void");
			}
			else
			{
				throw "Error Type: this Data type is not vaild keyword.";
			}
		}
		else if (_jackTokenizer->tokenType() == TokenType::IDENTIFIER)
		{
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();
		}
		else
		{
			throw "Error Type: this Data type is not valid.";
		}

		// subroutineName
		_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
		_jackTokenizer->advance();

		if (!_eat("(")) throw "Error symbol: Current token does not equal expected token.";

		CompileParameterList();

		if (!_eat(")")) throw "Error symbol: Current token does not equal expected token.";

		// subroutineBody
		CompileSubroutineBody();

		_outputFile << "</subroutineDec>" << endl; // close subroutineDec tage
	}

	void CompileParameterList()
	{
		_outputFile << "<parameterList>" << endl; // open parameterList tage

		while (_jackTokenizer->tokenType() == TokenType::KEYWORD || _jackTokenizer->tokenType() == TokenType::IDENTIFIER)
		{
			if (_jackTokenizer->tokenType() == TokenType::KEYWORD)
			{
				if (_jackTokenizer->keywordType() == KeywordType::INT)
				{
					_eat("int");
				}
				else if (_jackTokenizer->keywordType() == KeywordType::CHAR)
				{
					_eat("char");
				}
				else if (_jackTokenizer->keywordType() == KeywordType::BOOLEAN)
				{
					_eat("boolean");
				}
				else
				{
					throw "Error Type: this Data type is not vaild keyword.";
				}
			}
			else if (_jackTokenizer->tokenType() == TokenType::IDENTIFIER)
			{
				_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
				_jackTokenizer->advance();
			}
			else
			{
				throw "Error Type: this Data type is not valid.";
			}

			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();

			if (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ',')) break;

			_eat(",");
		}

		_outputFile << "</parameterList>" << endl; // close parameterList tage
	}

	void CompileSubroutineBody()
	{
		_outputFile << "<subroutineBody>" << endl; // open subroutineBody tage

		if (!_eat("{")) throw "Error symbol: Current token does not equal expected token.";

		while (_jackTokenizer->tokenType() == TokenType::KEYWORD && _jackTokenizer->keywordType() == KeywordType::VAR)
		{
			CompileVarDec();
		}

		CompileStatements();

		if (!_eat("}")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</subroutineBody>" << endl; // close subroutineBody tage
	}

	void CompileVarDec()
	{
		_outputFile << "<varDec>" << endl; // open varDec tage

		if (!_eat("var")) throw "Error keyword: Current token does not equal expected token.";

		// type
		if (_jackTokenizer->tokenType() == TokenType::KEYWORD)
		{
			if (_jackTokenizer->keywordType() == KeywordType::INT)
			{
				_eat("int");
			}
			else if (_jackTokenizer->keywordType() == KeywordType::CHAR)
			{
				_eat("char");
			}
			else if (_jackTokenizer->keywordType() == KeywordType::BOOLEAN)
			{
				_eat("boolean");
			}
			else
			{
				throw "Error Type: this Data type is not vaild keyword.";
			}
		}
		else if (_jackTokenizer->tokenType() == TokenType::IDENTIFIER)
		{
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();
		}
		else
		{
			throw "Error Type: this Data type is not valid.";
		}

		// varName 1 or more
		while (true)
		{
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();

			if (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ',')) break;

			_eat(",");
		}

		if (!_eat(";")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</varDec>" << endl; // close varDec tage
	}

	// Statements

	void CompileStatements()
	{
		_outputFile << "<statements>" << endl; // open statements tage

		while (_jackTokenizer->tokenType() == TokenType::KEYWORD)
		{

			if (_jackTokenizer->keywordType() == KeywordType::LET)
			{
				CompileLet();
			}
			else if (_jackTokenizer->keywordType() == KeywordType::IF)
			{
				CompileIf();
			}
			else if (_jackTokenizer->keywordType() == KeywordType::WHILE)
			{
				CompileWhile();
			}
			else if (_jackTokenizer->keywordType() == KeywordType::DO)
			{
				CompileDo();
			}
			else if (_jackTokenizer->keywordType() == KeywordType::RETURN)
			{
				CompileReturn();
			}
			else
			{
				break;
			}
		}

		_outputFile << "</statements>" << endl; // close statements tage
	}

	void CompileLet()
	{
		_outputFile << "<letStatement>" << endl; // open letStatement tage

		if (!_eat("let")) throw "Error keyword: Current token does not equal expected token.";

		// varName
		_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
		_jackTokenizer->advance();

		// [ expression ]

		if (_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == '[')
		{
			if (!_eat("[")) throw "Error keyword: Current token does not equal expected token.";

			CompileExpression();

			if (!_eat("]")) throw "Error keyword: Current token does not equal expected token.";
		}

		if (!_eat("=")) throw "Error symbol: Current token does not equal expected token.";

		CompileExpression();

		if (!_eat(";")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</letStatement>" << endl; // close letStatement tage
	}

	void CompileIf()
	{
		_outputFile << "<ifStatement>" << endl; // open ifStatement tage

		if (!_eat("if")) throw "Error keyword: Current token does not equal expected token.";

		if (!_eat("(")) throw "Error symbol: Current token does not equal expected token.";

		CompileExpression();

		if (!_eat(")")) throw "Error symbol: Current token does not equal expected token.";

		if (!_eat("{")) throw "Error symbol: Current token does not equal expected token.";

		CompileStatements();

		if (!_eat("}")) throw "Error symbol: Current token does not equal expected token.";

		if (_jackTokenizer->tokenType() == TokenType::KEYWORD && _jackTokenizer->keywordType() == KeywordType::ELSE)
		{
			if (!_eat("else")) throw "Error keyword: Current token does not equal expected token.";

			if (!_eat("{")) throw "Error symbol: Current token does not equal expected token.";

			CompileStatements();

			if (!_eat("}")) throw "Error symbol: Current token does not equal expected token.";
		}

		_outputFile << "</ifStatement>" << endl; // close ifStatement tage
	}

	void CompileWhile()
	{
		_outputFile << "<whileStatement>" << endl; // open whileStatement tage

		if (!_eat("while")) throw "Error keyword: Current token does not equal expected token.";

		if (!_eat("(")) throw "Error symbol: Current token does not equal expected token.";

		CompileExpression();

		if (!_eat(")")) throw "Error symbol: Current token does not equal expected token.";

		if (!_eat("{")) throw "Error symbol: Current token does not equal expected token.";

		CompileStatements();

		if (!_eat("}")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</whileStatement>" << endl; // close whileStatement tage
	}

	void CompileDo()
	{
		_outputFile << "<doStatement>" << endl; // open doStatement tage

		if (!_eat("do")) throw "Error keyword: Current token does not equal expected token.";

		_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
		_jackTokenizer->advance();

		if (_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == '.')
		{
			_eat(".");
			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();
		}

		if (!_eat("(")) throw "Error symbol: Current token does not equal expected token.";

		CompileExpressionList();

		if (!_eat(")")) throw "Error symbol: Current token does not equal expected token.";

		if (!_eat(";")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</doStatement>" << endl; // close doStatement tage
	}

	void CompileReturn()
	{
		_outputFile << "<returnStatement>" << endl; // open returnStatement tage

		if (!_eat("return")) throw "Error keyword: Current token does not equal expected token.";

		if (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ';'))
		{
			CompileExpression();
		}

		if (!_eat(";")) throw "Error symbol: Current token does not equal expected token.";

		_outputFile << "</returnStatement>" << endl; // close returnStatement tage
	}

	// Expression

	void CompileExpression()
	{
		_outputFile << "<expression>" << endl; // open expression tage

		CompileTerm();

		_outputFile << "</expression>" << endl; // close expression tage
	}

	void CompileTerm()
	{
		_outputFile << "<term>" << endl; // open term tage

		switch (_jackTokenizer->tokenType())
		{
		case KEYWORD:
			if (!(_eat("true") || _eat("false") || _eat("null") || _eat("this"))) throw "Error keyword: Current token does not equal expected token.";
			break;
		case IDENTIFIER:

			// more here must be added

			_outputFile << "<identifier> " + _jackTokenizer->identifier() + " </identifier>" << endl;
			_jackTokenizer->advance();
			break;
		case INT_CONST:
			_outputFile << "<integerConstant> " + to_string(_jackTokenizer->intVal()) + " </integerConstant>" << endl;
			_jackTokenizer->advance();
			break;
		case STRING_CONST:
			_outputFile << "<stringConstant> " + _jackTokenizer->stringVal() + " </stringConstant>" << endl;
			_jackTokenizer->advance();
			break;
		default:
			throw "Error expression: Current expression is not defined.";
			break;
		}

		_outputFile << "</term>" << endl; // close term tage
	}

	void CompileExpressionList()
	{
		_outputFile << "<expressionList>" << endl; // open expressionList tage

		if  (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ')'))
		{
			while (true)
			{
				CompileExpression();

				if (!(_jackTokenizer->tokenType() == TokenType::SYMBOL && _jackTokenizer->symbol() == ',')) break;

				_eat(",");
			}
		}

		_outputFile << "</expressionList>" << endl; // close expressionList tage
	}
};

