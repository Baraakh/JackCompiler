#include <iostream>
#include <fstream>
#include <string>
#include "JackTokenizer.h"

using namespace std;

int main()
{

    std::string filePath = ""; // Change this to the path of your file
    
    JackTokenizer jackTokenizer(filePath);

    fstream output;
    output.open("Toutput.txt", ios::app | ios::out);

    output << "<tokens>" << endl;

    while (jackTokenizer.hasMoreTokens())
    {
        
        switch (jackTokenizer.tokenType())
        {
        case KEYWORD:
            output << "<keyword> " + jackTokenizer.keywordValue() + " </keyword>" << endl;
            break;
        case SYMBOL:

            if (jackTokenizer.symbol() == '<')
            {
                output << "<symbol> &lt; </symbol>" << endl;
            }
            else if (jackTokenizer.symbol() == '>')
            {
                output << "<symbol> &gt; </symbol>" << endl;
            }
            else if (jackTokenizer.symbol() == '"')
            {
                output << "<symbol> &quot; </symbol>" << endl;
            }
            else if (jackTokenizer.symbol() == '&')
            {
                output << "<symbol> &amp; </symbol>" << endl;
            }
            else
            {
                output << "<symbol> " << jackTokenizer.symbol() << " </symbol>" << endl;
            } 

            break;
        case IDENTIFIER:
            output << "<identifier> " + jackTokenizer.identifier() + " </identifier>" << endl;
            break;
        case INT_CONST:
            output << "<integerConstant> " + to_string(jackTokenizer.intVal()) + " </integerConstant>" << endl;
            break;
        case STRING_CONST:
            output << "<stringConstant> " + jackTokenizer.stringVal() + " </stringConstant>" << endl;
            break;
        }
        jackTokenizer.advance();
    }

    output << "</tokens>" << endl;

	return 0;
}
