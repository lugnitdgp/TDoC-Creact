#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

// class for the transpiler
class Transpiler {
private:
    std::unordered_map<std::string, std::string> dataMapper;    // symbol table to store all the keyword mappings
    std::vector<std::string> setParserData;                     // vector to store the transpiled keywords after parsing
    std::unordered_map<std::string, std::string> variableMapper;
public:
    Transpiler() {  // store the keyword mappings
        dataMapper.insert({"htpl", "#include <stdio.h>\n"});
        dataMapper.insert({"main", "void main(){\n"});
        dataMapper.insert({"log", "printf()"});
        dataMapper.insert({"/", "}"});
        dataMapper.insert({"/log", ";"});
        dataMapper.insert({"in", "int"});
        dataMapper.insert({"ch", "char"});
        dataMapper.insert({"fl", "float"});
        dataMapper.insert({"int", "%d"});
        dataMapper.insert({"char", "%c"});
        dataMapper.insert({"float", "%f"});
        dataMapper.insert({"take", "scanf()"});
    }
    // function to remove the spaces from a line
    std::string removeSpaces(std::string getData) {  
        long long int ptr1 = 0, ptr2 = getData.length() - 1;
        while (getData[ptr1] == ' ' || getData[ptr2] == ' ') {
            if (getData[ptr1] == ' ' )
                ++ptr1;
            if (getData[ptr2] == ' ')
                --ptr2;
        }
        return getData.substr(ptr1, ptr2 - ptr1 + 1);
    }
    // function to get keyword by removing the angular braces
    std::string stripBraces(std::string getData) {
        return getData.substr(1, getData.find('>') - 1);
    }
    // function to add spaces between separators for i/o, var declaration tags
    std::string spacify(std::string getData) {
        std::string newData = "";
        for (int i = 0; i < getData.length(); ++i) {
            if (getData[i] == '=' || getData[i] == ',') {
                newData += ' ';
                newData += getData[i];
                newData += ' ';
            } else {
                newData += getData[i];
            }
        }
        return newData;
    }
    // parses an input tag or initialization tag(updates the variable map), and returns the tokens
    std::vector<std::string> IOParser(std::string getData, bool io = false) {
        getData = spacify(getData) + " ,";
        std::vector<std::string> tokens;
        std::stringstream dataStream(getData);
        std::string word;
        while (dataStream >> word) {    // tokenize using stringstream
            tokens.push_back(word);
        }
        if (io == false) {
            for (int i = 1; i < tokens.size(); ++i) {
                // store the data type for variables
                if (tokens[i] == "=" || (tokens[i] == "," && (i < 2 || tokens[i - 2] != "="))) {
                    variableMapper.insert({tokens[i - 1], dataMapper.find(dataMapper.find(tokens[0])->second)->second});
                }
            }
        }
        return tokens;
    }
    // function which parses a line
    // returns True if successfully parsed
    // returns False if error encountered from invalid tags
    bool Parser(std::string getData, int lineNo, bool removeSpace = true) {
        if (removeSpace == true)   // if new line
            getData = removeSpaces(getData);
        // std::cout << getData << "\n";
        if (getData[0] == '<') {    // if it's a tag
            if (getData[getData.length() - 1] == '>' && getData[getData.length() - 2] == '/') { // if it's a self closing tag
                getData = removeSpaces(getData.substr(1, getData.length() - 3));
                if (dataMapper.find(getData.substr(0, 2)) != dataMapper.end()) {   // check if variable declaration or not
                    setParserData.push_back(dataMapper.find(getData.substr(0, 2))->second + getData.substr(2) + ";");
                    IOParser(getData);
                } else if (getData.length() >= 4 && getData.substr(0, 4) == "take") {   // check for input tag
                    std::vector<std::string> tokens = IOParser(getData, true);  // get the tokens
                    std::string formatSpecifier = "", varNames = "";
                    for (int i = 1; i < tokens.size(); ++i) {   // build the parameters for scanf
                        if (tokens[i] == ",") {
                            if (variableMapper.find(tokens[i - 1]) != variableMapper.end()) {
                                formatSpecifier += variableMapper.find(tokens[i - 1])->second;
                                if (varNames != "") {
                                    varNames += ",";
                                }
                                varNames += ("&" + tokens[i - 1]);
                            } else {    // undeclared variable input, error
                                std::cout << "Error at Line " << lineNo << ": Input for undeclared variable \'" << tokens[i] << "\'.\n";
                                return false;
                            }
                        }
                    }
                    setParserData.push_back(dataMapper.find(tokens[0])->second);
                    setParserData.push_back("\"" + formatSpecifier + "\"," + varNames);
                } else {    // invalid self closing tag
                    std::cout << "Error at Line " << lineNo << ": Couldn\'t recognize tag \'" << getData << "\'.\n";
                    return false;
                }

            } else if (getData[1] != '/') {    // if opening tag
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
            if (index != std::string::npos && index + 1 < getData.length()) {
                getData = getData.substr(index + 1);
                Parser(getData, lineNo, true);
            }
        } else {    // if not a tag
            int index = getData.find('<');  // to print only text block, preceding any closing tag
            if (setParserData.back() == "printf()") {   // if following a print tag
                setParserData.push_back(getData.substr(0, index));
            } else {    // if part of a text block
                setParserData.back() += (" " + getData.substr(0, index));    
            }
            // pass the rest of the line(if any) after this text block for parsing
            if (index != std::string::npos && index < getData.length()) {
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
                std::string formatSpecifiers = "", varNames = "", varTemp = "";
                for (int j = 0; j < setParserData[i + 1].length(); ++j) {
                    if (j + 1 < setParserData[i + 1].length() && setParserData[i + 1].substr(j, 2) == "${") {
                        int pos = setParserData[i + 1].find('}', j);
                        varTemp = setParserData[i + 1].substr(j + 2, pos - (j + 2));
                        if (variableMapper.find(varTemp) != variableMapper.end()) {
                            formatSpecifiers += variableMapper.find(varTemp)->second;
                            if (varNames.length() > 0) {
                                varNames += ",";
                            }
                            varNames += varTemp;
                        } else {
                            // printing as it is, not error
                            formatSpecifiers += setParserData[i + 1].substr(j, pos - j + 1);
                        }
                        j = pos;
                    } else {
                        formatSpecifiers += setParserData[i + 1][j];
                    }
                }
                string_const = setParserData[i].substr(0, 7) + '"' + formatSpecifiers + '"' + (varNames != "" ? "," : "") + varNames + ")" + setParserData[i + 2];
                setParserData[i] = string_const;
                // erasing the text block and the ';' from the vector
                setParserData.erase(std::next(setParserData.begin(), i + 1), std::next(setParserData.begin(), i + 3));
            } else if (setParserData[i] == "scanf()") {
                string_const = setParserData[i].substr(0, 6) + setParserData[i + 1] + ");";
                setParserData[i] = string_const;
                setParserData.erase(setParserData.begin() + i + 1);
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