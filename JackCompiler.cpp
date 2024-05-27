#include <iostream>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;

int main()
{
	
	JackTokenizer* jackTokenizer = new JackTokenizer("fileName.jack");
	CompilationEngine compilationEngine(jackTokenizer, "fileName.xml");

	return 0;
}
