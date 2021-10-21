#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <stack>

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
        dataMapper.insert({"elif", "else if"});
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

    // check if it's a tag or not, parameter passed a string within angular braces, i.e. < ... >
    bool checkIfTag(std::string getTag) {
        getTag = removeSpaces(getTag.substr(1));
        if (getTag[0] == '?' || getTag[0] == '%')   // conditional or expression tag
            return true;
        else {
            std::string token = "";
            for (int i = 0; i < getTag.length(); ++i) { // extract the first token(the tag, supposedly)
                if (getTag[i] != ' ' && getTag[i] != '>')
                    token += getTag[i];
                else
                    break;
            }
            // std::cout << "INSIDE CHECKTAG: " << token << "\n";
            return (dataMapper.find(token) != dataMapper.end());    // check in symbol table
        }
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
        // std::cout << getData << " (IN PARSER)\n";

        if (getData[0] == '<') {    // if it's a tag
            if (getData.find('>') != std::string::npos && getData[getData.find('>') - 1] == '/') { // if it's a self closing tag
                std::string getTagData = removeSpaces(getData.substr(1, getData.find('>') - 2));
                if (getTagData.length() >= 2 && dataMapper.find(getTagData.substr(0, 2)) != dataMapper.end()) {   // check if variable declaration or not
                    setParserData.push_back(dataMapper.find(getTagData.substr(0, 2))->second + getTagData.substr(2) + ";");
                    IOParser(getTagData);

                } else if (getTagData.length() >= 4 && getTagData.substr(0, 4) == "take") {   // check for input tag
                    std::vector<std::string> tokens = IOParser(getTagData, true);  // get the tokens
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
                    std::cout << "Error at Line " << lineNo << ": Couldn\'t recognize tag \'" << getTagData << "\'.\n";
                    return false;
                }

            } else if (getData[1] == '%') { // possibility of an arithmetic expression
                int index = getData.find('>') - 1, pos = -1;
                if (index != std::string::npos) {
                    while (index > 1) {
                        if (getData[index] == '%') {    // find closing % for valid arithmetic expression tag
                            pos = index;
                            break;
                        } else if (getData[index] != ' ') { // wrong syntax
                            std::cout << "Error at Line " << lineNo << ": Invalid Syntax for expression. \n";
                            return false;
                        }
                        --index;
                    }
                }
                if (pos != -1) {    // valid expression
                    std::string getTagData = removeSpaces(getData.substr(2, pos - 2));
                    setParserData.push_back(getTagData + ";");
                } else {    // invalid expression
                    std::cout << "Error at Line " << lineNo << ": Invalid Syntax for expression. \n";
                    return false;
                }

            } else if (getData[1] == '?') {     // possibility of an conditional statement
                if (conditionalBuilder(getData, lineNo) == false)
                    return false;
                return true;

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
                } else if (getData.find('>') != std::string::npos && dataMapper.find(getData.substr(2, getData.find('>') - 2)) != dataMapper.end()){   // if other valid tag closing
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
                if (Parser(getData, lineNo, true) == false)
                    return false;
            }

        } else if (getData[0] == '?') {  // closing of a conditional block
            if (conditionalBuilder(getData, lineNo) == false)
                return false;
            
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
                if (Parser(getData, lineNo) == false)
                    return false;
            }

        }
        return true;
    }

    // check if a condition of a conditional statement is valid or not
    bool conditionChecker(std::string cond) {
        cond = removeSpaces(cond);
        // std::cout << cond << " (INSIDE CONDITION CHECKER)\n";
        std::stack<char> s;
        for (char ch : cond) {
            if (ch == '(') {
                s.push(ch);
            } else if (ch == ')') {
                if (s.empty())
                    return false;
                s.pop();
            }
        }
        return (s.empty());
    }

    // function to find index of next tag in the line
    int findNextTag(std::string getData) {
        int idx = getData.find('<');
        while (idx != std::string::npos) {
            // std::cout << "CurrTag: " << getData.substr(idx) << "\n";
            if (checkIfTag(getData.substr(idx)))
                return idx;
            idx = getData.find('<', idx + 1);
        }
        return idx;
    }

    // function to parse conditional statements(both inline and multiple lines)
    bool conditionalBuilder(std::string getData, int lineNo) {
        int neslvl = 0, pos = 0;
        for (int i = 0; i < getData.length(); ++i) {
            if (getData[i] == '?') {
                ++neslvl;
            } else if (getData[i] == 'i' || getData[i] == 'e') {
                break;
            }
            ++pos;
        }
        int index = getData.length() - 1;
        while (index > pos) {   // if closing also in same line
            if (getData[index] == '>')
                break;
            --index;
        }
        int len = getData.length() - pos;
        bool closeHere = false;
        if (((getData[0] == '<' && index > neslvl + 1) || (getData[0] != '<' && index > 0)) && getData[index - 1] == '?')    // check if closing in same line or not
            len = index - pos - 1 , closeHere = true;
        if (getData[0] == '<') {    // if opening conditional
            std::string getTagData = removeSpaces(getData.substr(pos, len));
            if (getTagData.length() >= 2 && getTagData.substr(0, 2) == "if") {  // for if block
                int idx = findNextTag(getTagData); 
                if (idx == std::string::npos)
                    idx = getTagData.length();
                if (conditionChecker(getTagData.substr(2, idx - 2)) == false) { // invalid condition
                    std::cout << "Error at Line " << lineNo << ": Invalid Conditional Statement.\n";
                    return false;
                } else {    // parse the if statement, and pass the rest(if any) for normal parsing
                    setParserData.push_back(getTagData.substr(0, idx) + "{");
                    if (getTagData.length() > idx) {
                        if (Parser(getTagData.substr(idx), lineNo) == false)
                            return false;
                    }
                }

            } else if (getTagData.length() >= 4 && getTagData.substr(0, 4) == "elif") { // for else if block
                int idx = findNextTag(getTagData);
                if (idx == std::string::npos)
                    idx = getTagData.length();
                if (conditionChecker(getTagData.substr(4, idx - 4)) == false) { // invalid condition
                    std::cout << "Error at Line " << lineNo << ": Invalid Conditional Statement.\n";
                    return false;
                } else {    // parse the else if statement, and pass the rest(if any) for normal parsing
                    setParserData.push_back(dataMapper.find(getTagData.substr(0, 4))->second + getTagData.substr(4, idx - 4) + "{"); 
                    if (getTagData.length() > idx) {
                        if (Parser(getTagData.substr(idx), lineNo) == false)
                            return false;
                    }
                }

            } else if (getTagData.length() >= 4 && getTagData.substr(0, 4) == "else") { // for else block
                setParserData.push_back("else{");
                if (getTagData.length() > 4) {  // pass the rest(if any) for normal parsing
                    if (Parser(getTagData.substr(4), lineNo) == false)
                        return false;
                }

            } else if (getTagData.length() >= 1 && getTagData[0] == '>') {  // if closing conditional
                setParserData.push_back("}");

            } else {    // invalid conditional
                std::cout << "Error at Line " << lineNo << ": Invalid Conditional Statement.\n";
                return false;
            }
        }
        if (closeHere == true) { // add closing tag for inline conditionals
            setParserData.push_back("}");
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

            } else if (setParserData[i] == "scanf()") { // whenever input statement is encountered
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
            // getSyntax = T.removeSpaces(getSyntax);  // CHECK CORRECTNESS FOR TEXT BLOCKS
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