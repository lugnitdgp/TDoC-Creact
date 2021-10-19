#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

// class for the transpiler
class Transpiler {
private:
    std::unordered_map<std::string, std::string> dataMapper;    // symbol table to store all the keyword mappings
    std::vector<std::string> setParserData;                     // vector to store the transpiled keywords after parsing
public:
    Transpiler() {  // store the keyword mappings
        dataMapper.insert({"htpl", "#include <stdio.h>\n"});
        dataMapper.insert({"main", "void main(){\n"});
        dataMapper.insert({"log", "printf()"});
        dataMapper.insert({"/", "}"});
        dataMapper.insert({"/log", ";"});
    }
    // function to remove the front spaces from a line
    std::string removeFrontSpaces(std::string getData) {  
        for (int i = 0; i < getData.length(); ++i) {
            if (getData[i] != ' ') {
                getData = getData.substr(i, getData.length() - i);
                break;
            }
        }
        return getData;
    }
    // function to get keyword by removing the angular braces
    std::string stripBraces(std::string getData) {
        return getData.substr(1, getData.find('>') - 1);
    }
    // function which parses a line
    // returns True if successfully parsed
    // returns False if error encountered from invalid tags
    bool Parser(std::string getData, int lineNo, bool removeSpaces = true) {
        if (removeSpaces == true)   // if new line
            getData = removeFrontSpaces(getData);
        // std::cout << getData << "\n";
        if (getData[0] == '<') {    // if it's a tag
            if (getData[1] != '/') {    // if opening tag
                std::string getTag = stripBraces(getData);
                if (dataMapper.find(getTag) != dataMapper.end()) {
                    setParserData.push_back(dataMapper.find(getTag)->second);
                } else {    // wrong tag, print error and exit
                    std::cout << "Error at Line " << lineNo << ": Couldn\'t recognize tag \'" << getTag << "\'.\n";
                    return false;
                }
            } else {    // if closing tag
                if (getData.substr(2, 3) == "log") {    // if print tag closing
                    setParserData.push_back(dataMapper.find("/log")->second);
                } else if (dataMapper.find(getData.substr(2, getData.find('>') - 2)) != dataMapper.end()){   // if other valid tag closing
                    setParserData.push_back(dataMapper.find("/")->second);
                } else {    // if wrong closing tag
                    std::cout << "Error at Line " << lineNo << ": Couldn\'t recognize tag \'" << getData.substr(2, getData.find('>') - 2) << "\'.\n";
                    return false;
                }
            }
            // pass the rest of the line(if any) after this tag for parsing
            int index = getData.find('>');
            if (index + 1 < getData.length()) {
                getData = getData.substr(index + 1);
                Parser(getData, lineNo, false);
            }
        } else {    // if not a tag
            int index = getData.find('<');  // to print only text block, preceding any closing tag
            if (setParserData.back() == "printf()") {   // if following a print tag
                setParserData.push_back(getData.substr(0, index));
            } else {    // if part of a text block
                setParserData.back() += (" " + getData.substr(0, index));    
            }
            // pass the rest of the line(if any) after this text block for parsing
            if (index < getData.length()) {
                getData = getData.substr(index);
                Parser(getData, lineNo);
            }
        }
        return true;
    }
    // function to organize the print statements
    void printParser() {
        std::string string_const;
        for (int i = 0; i < setParserData.size(); ++i) {
            if (setParserData[i] == "printf()") {   // whenever print statement is encountered
                // enclosing the text block within the printf function block and adding ';' at the end
                string_const = setParserData[i].substr(0, 7) + '"' + setParserData[i + 1] + '"' + ")" + setParserData[i + 2];
                setParserData[i] = string_const;
                // erasing the text block and the ';' from the vector
                setParserData.erase(std::next(setParserData.begin(), i + 1), std::next(setParserData.begin(), i + 3));
            }
        }
    }
    // function to write the code in the C file
    void writeCode() {
        std::ofstream writeFile("output.c");
        for (int i = 0; i < setParserData.size() - 1; ++i) {
            writeFile << setParserData[i] << "\n";
        }
    }
};

// class to simulate the transpilation
class Runner {
private:
    Transpiler T;
    std::string inputFile;
public:
    Runner() {
        inputFile = "input.html";
    }
    Runner(const char* filename) {
        inputFile = filename;
    }
    // execute
    void run() {
        std::string getSyntax;
        std::ifstream readData(inputFile);
        int lineNo = 0;
        while (getline(readData, getSyntax)) {  // read each line from the Creact code
            ++lineNo;
            if (T.Parser(getSyntax, lineNo) == false) { // parse each line
                return;
            }
        }
        T.printParser();
        T.writeCode();
    }
};

int main (int argc, char const *argv[]) {
    if (argc == 1) {
        Runner R;
        R.run();
    } else if (argc == 2) {
        Runner R(argv[1]);
        R.run();
    }
}