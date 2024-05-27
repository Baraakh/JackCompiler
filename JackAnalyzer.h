#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
using namespace std;
namespace fs = std::filesystem;

class JackAnalyzer
{
	JackTokenizer* _jackTokenizer = nullptr;
	CompilationEngine* _compilationEngine = nullptr;

public:

	JackAnalyzer(string PATH)
	{    
        fs::path pathObj(PATH);

        if (fs::exists(pathObj)) {
            if (fs::is_regular_file(pathObj)) {
                cout << PATH << " is a regular file." << endl;

                string OUTPUT_FILE_PATH;

                // genrating the output file path
                OUTPUT_FILE_PATH = PATH.substr(0, PATH.find_first_of("."));
                OUTPUT_FILE_PATH += ".xml";

                _jackTokenizer = new JackTokenizer(PATH);
                _compilationEngine = new CompilationEngine(_jackTokenizer, OUTPUT_FILE_PATH);

                cout << "OUTPUT FilePath: " << OUTPUT_FILE_PATH << endl;

                delete _jackTokenizer, _compilationEngine;
            }
            else if (fs::is_directory(pathObj)) {
                std::cout << PATH << " is a directory." << std::endl;

                for (const auto& file : fs::directory_iterator(pathObj))
                {
                    if (fs::is_regular_file(file)) {
                        if (file.path().extension() == ".jack") {

                            string OUTPUT_FILE_PATH;

                            // genrating the output file path
                            OUTPUT_FILE_PATH = file.path().string().substr(0, file.path().string().find_first_of("."));
                            OUTPUT_FILE_PATH += ".xml";

                            _jackTokenizer = new JackTokenizer(file.path().string());
                            _compilationEngine = new CompilationEngine(_jackTokenizer, OUTPUT_FILE_PATH);

                            delete _jackTokenizer, _compilationEngine;
                        }
                    }
                }

                cout << "OUTPUT FilePath: " << pathObj.string() + "/" + pathObj.filename().string() + ".xml" << endl;
            }
            else {
                std::cout << PATH << " is neither a regular file nor a directory." << std::endl;
            }
        }
        else {
            std::cout << PATH << " does not exist." << std::endl;
        }

        cout << "\nTranslating the Jack source code into XML format has been successfully done :)." << endl;

	}
};

