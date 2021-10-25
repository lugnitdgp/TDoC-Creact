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
    std::unordered_map<std::string, std::string> variableMapper;// store table for declared variables
    std::unordered_map<std::string, int> variableFreq;          // store count of variables with a shared name, to handle scope issues in a function
    std::vector<std::string> functionHeaders;                   // store function signatures
    std::vector<std::string> functionScopeVars;                 // to maintain scope of variables inside function
    std::vector<std::unordered_map<std::string, std::string>> functionScopeVarMappers;   // to track scope of variables inside all functions
    std::unordered_map<std::string, std::string> scopeVarMapper;    // temp mapper for current scope
    int functionNo;                                             // to store current function number
    bool insideFunction;                                        // state to determine current parsing is inside a function or not
    bool dynamicNeeded;                                         // to check if dynamic memory allocation modules needed or not
    int countVector;                                            // to keep count of vectors declared till now
    std::unordered_map<std::string, std::string> vectorMapper;  // to map vectors with it's count variables
public:
    Transpiler() {  // store the keyword mappings
        insideFunction = dynamicNeeded = false;
        functionNo = countVector = 0;
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
        dataMapper.insert({"f", "for"});
        dataMapper.insert({"w", "while"});
        dataMapper.insert({"fx", "func"});  // for the closing tag handling
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
        if (getTag[0] == '?' || getTag[0] == '%' || getTag[0] == '#')   // conditional or expression or loop tag
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
                    // for scope handling
                    ++variableFreq[tokens[i - 1]];
                    if (insideFunction == true) {
                        functionScopeVars.push_back(tokens[i - 1]);
                        scopeVarMapper.insert({tokens[i - 1], dataMapper.find(dataMapper.find(tokens[0])->second)->second});
                    }
                }
            }
        }
        return tokens;
    }

    // function to handle the dynamic memory allocation tasks
    bool memoryPlay(std::string getData, int lineNo) {
        getData = removeSpaces(getData);
        if (getData.substr(0, 6) == "stream") { // vector declaration
            dynamicNeeded = true;
            std::string dataType = getData.substr(getData.find('(') + 1, getData.find(')') - getData.find('(') - 1);
            if (dataMapper.find(dataType) != dataMapper.end()) {
                std::string varName = removeSpaces(getData.substr(getData.find(')') + 1));
                std::string sizeName = varName + std::to_string(countVector);
                std::string countName = "c" + std::to_string(countVector);
                vectorMapper.insert({varName, countName});
                vectorMapper.insert({countName, sizeName});

                variableMapper.insert({varName, dataMapper.find(dataType)->second});   // store data types and not format specifiers for vectors, for function choosing
                variableMapper.insert({countName, dataMapper.find(dataMapper.find("in")->second)->second});
                variableMapper.insert({sizeName, dataMapper.find(dataMapper.find("in")->second)->second});
                // for scope handling
                ++variableFreq[varName]; ++variableFreq[countName]; ++variableFreq[sizeName];
                if (insideFunction == true) {
                    functionScopeVars.push_back(varName);
                    scopeVarMapper.insert({varName, dataMapper.find(dataType)->second});
                    functionScopeVars.push_back(countName);
                    scopeVarMapper.insert({countName, dataMapper.find(dataMapper.find("in")->second)->second});
                    functionScopeVars.push_back(sizeName);
                    scopeVarMapper.insert({sizeName, dataMapper.find(dataMapper.find("in")->second)->second});
                }

                setParserData.push_back("int " + countName + " = 0;");
                setParserData.push_back("int " + sizeName + " = SIZE;");
                std::string instance = dataMapper.find(dataType)->second + " *" + varName + " = (" + dataMapper.find(dataType)->second + "*)malloc(sizeof(" + dataMapper.find(dataType)->second + ")*" + sizeName + ");";
                setParserData.push_back(instance);

                ++countVector;
            } else {    // invalid data type
                std::cout << "Error at Line " << lineNo << ": Invalid data type \'" << dataType << "\'.\n";
                return false;
            }

        } else {
            if (getData.substr(getData.find('.') + 1, 4) == "plus") {   // for adding elements
                std::string checkoutCall = getData.substr(0, getData.find('.')) + "=" + "checkout" + variableMapper.find(getData.substr(0, getData.find('.')))->second + "(" + vectorMapper.find(getData.substr(0, getData.find('.')))->second + ",&" + vectorMapper.find(vectorMapper.find(getData.substr(0, getData.find('.')))->second)->second + ',' + getData.substr(0, getData.find('.')) + ");";
                std::string updateLine = "*(" + getData.substr(0, getData.find('.')) + "+" + vectorMapper.find(getData.substr(0, getData.find('.')))->second + "++)=" + getData.substr(getData.find('(') + 1, getData.length() - getData.find('(') - 2) + ";";
                setParserData.push_back(checkoutCall);
                setParserData.push_back(updateLine);

            }
            else if (getData.substr(getData.find('.') + 1, 5) == "minus") { // for removing elements
                setParserData.push_back(vectorMapper.find(getData.substr(0, getData.find('.')))->second + "--" + ';');

            }
            else if (getData.substr(getData.find('.') + 1, 4) == "show") {  // for displaying all elements
                setParserData.push_back("show" + variableMapper.find(getData.substr(0, getData.find('.')))->second + "(" + getData.substr(0, getData.find('.')) + "," + vectorMapper.find(getData.substr(0, getData.find('.')))->second + ");");

            }
        }
        return true;
    }

    // function which parses a line
    // returns True if successfully parsed
    // returns False if error encountered from invalid tags
    bool Parser(std::string getData, int lineNo, bool removeSpace = true) {
        if (removeSpace == true)   // if new line
            getData = removeSpaces(getData);
        // std::cerr << getData << " (IN PARSER)\n";

        if (getData[0] == '<') {    // if it's a tag
            if (getData.find('>') != std::string::npos && getData[getData.find('>') - 1] == '/') { // if it's a self closing tag
                std::string getTagData = removeSpaces(getData.substr(1, getData.find('>') - 2));
                if (getTagData.length() >= 2 && getTagData.substr(0, 2) == "fx") {  // function call
                    setParserData.push_back(getTagData.substr(2, getTagData.length() - 2) + ";");

                } else if (getTagData.length() >= 5 && getTagData.substr(0, 5) == "throw") {  // return statement(throw tag)
                    setParserData.push_back("return " + getTagData.substr(5, getTagData.length() - 5) + ";");

                } else if (getTagData.length() >= 2 && dataMapper.find(getTagData.substr(0, 2)) != dataMapper.end()) {   // check if variable declaration or not
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

            } else if (getData.length() >= 2 && getData.substr(0, 2) == "<<") {  // dynamic memory allocation 
                int idx = getData.find('>');
                if (idx != std::string::npos && idx + 1 < getData.length() && getData[idx + 1] == '>') {    // valid dynamic memory allocation tag
                    if (memoryPlay(getData.substr(2, idx - 2), lineNo) == false)
                        return false;
                    getData = getData.substr(idx + 1);  // removing the <<...> part, for parsing the rest(if any)

                } else {    // invalid tag
                    std::cout << "Error at Line " << lineNo << ": Invalid syntax for memory allocation.\n";
                    return false;
                }

            } else if (removeSpaces(getData.substr(1, getData.find('>') - 2)).substr(0, 2) == "fx") {  // function signature
                insideFunction = true;
                std::string getTagData = removeSpaces(getData.substr(1, getData.find('>') - 1));
                std::vector<std::string> tokens;
                std::string newData = "";
                for (int i = 0; i < getTagData.length(); ++i) {    // spacify
                    if (getTagData[i] == '(' || getTagData[i] == ',' || getTagData[i] == ')')
                        newData += " ", newData += getTagData[i], newData += " ";
                    else 
                       newData += getTagData[i];
                }
                std::stringstream dataStream(newData);
                std::string word;
                while (dataStream >> word) {    // tokenize using stringstream
                    tokens.push_back(word);
                }
                for (int i = 0; i < tokens.size(); ++i) {    // convert the data types
                    if (dataMapper.find(tokens[i]) != dataMapper.end()) {
                        if (tokens[i] != "fx") {    // paramete variables adding
                            variableMapper.insert({tokens[i + 1], dataMapper.find(dataMapper.find(tokens[i])->second)->second});
                            scopeVarMapper.insert({tokens[i + 1], dataMapper.find(dataMapper.find(tokens[i])->second)->second});
                            // for scope handling
                            ++variableFreq[tokens[i + 1]];
                            if (insideFunction == true) {
                                functionScopeVars.push_back(tokens[i + 1]);
                            }
                        }
                        tokens[i] = dataMapper.find(tokens[i])->second;
                    }
                }
                std::string signature;
                for (int i = 1; i < tokens.size(); ++i) {   // build the function signature;
                    signature += tokens[i] + " ";
                }
                setParserData.push_back(dataMapper.find("fx")->second); // for scope handling in printParser
                setParserData.push_back(signature + "{");
                functionHeaders.push_back(signature + ";");

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

            } else if (getData[1] == '#') {     // possibility of a loop statement
                if (loopBuilder(getData, lineNo) == false)
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
                } else if (getData.substr(2, 2) == "fx") {  // function closing and scope handling
                    insideFunction = false;
                    ++functionNo;
                    functionScopeVarMappers.push_back(scopeVarMapper);
                    scopeVarMapper.clear();
                    for (std::string vars : functionScopeVars) {
                        --variableFreq[vars];
                        if (variableFreq[vars] == 0)
                            variableMapper.erase(vars);
                    }
                    functionScopeVars.clear();
                    setParserData.push_back(dataMapper.find("/")->second);

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

        } else if (getData[0] == '?') {     // closing of a conditional block
            if (conditionalBuilder(getData, lineNo) == false)
                return false;
            
        } else if (getData[0] == '#') {     // closing of a loop block
            if (loopBuilder(getData, lineNo) == false)
                return false;
            return true;
            
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

        // discard all the '?' in the closing conditional part (THIS PART WILL ALSO BE USED DURING ORDER VALIDATION FOR LATER)
        --index; --index;
        if (index >= 0 && getData[index] == '?')
            --index, --len;

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
        } else if (getData[0] == '?') {    // closing conditional
            int idx = getData.find('>');
            if (idx == std::string::npos) { // invalid closing tag
                std::cout << "Error at Line " << lineNo << ": Invalid Conditional Statement.\n";
                return false;
            } else {
                setParserData.push_back("}");
                if (Parser(getData.substr(idx + 1), lineNo) == false)
                    return false;
                return true;
            }
                
        }
        if (closeHere == true) { // add closing tag for inline conditionals
            setParserData.push_back("}");
        }
        return true;
    }

    // function to parse loop statements(both inline and multiple lines)
    bool loopBuilder(std::string getData, int lineNo) {
        int neslvl = 0, pos = 0;
        for (int i = 0; i < getData.length(); ++i) {
            if (getData[i] == '#') {
                ++neslvl;
            } else if (getData[i] == 'f' || getData[i] == 'w') {
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
        if (((getData[0] == '<' && index > neslvl + 1) || (getData[0] != '<' && index > 0)) && (index > 0 && getData[index - 1] == '#'))    // check if closing in same line or not
            len = index - pos - 1 , closeHere = true;
        
        // discard all the '?' in the closing loop part (THIS PART WILL ALSO BE USED DURING ORDER VALIDATION FOR LATER)
        --index; --index;
        if (index >= 0 && getData[index] == '?')
            --index, --len;

        if (getData[0] == '<') {    // if opening conditional
            std::string getTagData = removeSpaces(getData.substr(pos, len));
            if (getTagData.length() >= 1 && getTagData.substr(0, 1) == "f") {  // for for block
                int idx = findNextTag(getTagData); 
                if (idx == std::string::npos)
                    idx = getTagData.length();
                if (conditionChecker(getTagData.substr(1, idx - 1)) == false) { // invalid condition
                    std::cout << "Error at Line " << lineNo << ": Invalid For Statement.\n";
                    return false;
                } else {    // parse the for statement, and pass the rest(if any) for normal parsing
                    setParserData.push_back(dataMapper.find(getTagData.substr(0, 1))->second + getTagData.substr(1, idx - 1) + "{");
                    if (getTagData.length() > idx) {
                        if (Parser(getTagData.substr(idx), lineNo) == false)
                            return false;
                    }
                }

            } else if (getTagData.length() >= 1 && getTagData.substr(0, 1) == "w") { // for while block
                int idx = findNextTag(getTagData);
                if (idx == std::string::npos)
                    idx = getTagData.length();
                if (conditionChecker(getTagData.substr(1, idx - 1)) == false) { // invalid condition
                    std::cout << "Error at Line " << lineNo << ": Invalid While Statement.\n";
                    return false;
                } else {    // parse the while statement, and pass the rest(if any) for normal parsing
                    setParserData.push_back(dataMapper.find(getTagData.substr(0, 1))->second + getTagData.substr(1, idx - 1) + "{"); 
                    if (getTagData.length() > idx) {
                        if (Parser(getTagData.substr(idx), lineNo) == false)
                            return false;
                    }
                }

            } else if (getTagData.length() >= 1 && getTagData[0] == '>') {  // if closing loop
                setParserData.push_back("}");

            } else {    // invalid loop
                std::cout << "Error at Line " << lineNo << ": Invalid Loop Statement.\n";
                return false;
            }
        } else if (getData[0] == '#') {    // closing loop
            int idx = getData.find('>');
            if (idx == std::string::npos) { // invalid closing tag
                std::cout << "Error at Line " << lineNo << ": Invalid Loop Statement.\n";
                return false;
            } else {
                setParserData.push_back("}");
                if (Parser(getData.substr(idx + 1), lineNo) == false)
                    return false;
                return true;
            }
                
        }
        if (closeHere == true) { // add closing tag for inline loop
            setParserData.push_back("}");
        }
        return true;
    }

    // function to tokenize expressions in print statements
    std::vector<std::string> tokenizeExpression(std::string getData) {
        std::vector<std::string> tokens;
        std::string newData = "";
        for (int i = 0; i < getData.length(); ++i) {    // spacify
            if (isalnum(getData[i]) || getData[i] == '_')
                newData += getData[i];
            else 
                newData += " ", newData += getData[i], newData += " ";
        }
        std::stringstream dataStream(newData);
        std::string word;
        while (dataStream >> word) {    // tokenize using stringstream
            tokens.push_back(word);
        }
        return tokens;
    }

    // function to organize the print statements
    void printParser() {
        std::string string_const;
        int funcIndex = -1;
        for (int i = 0; i < setParserData.size(); ++i) {
            if (setParserData[i].length() >= 4 && setParserData[i] == "func") { // scope handling in functions
                ++funcIndex;
                setParserData.erase(setParserData.begin() + i);

            } else if (setParserData[i] == "printf()") {   // whenever print statement is encountered
                // enclosing the text block within the printf function block and adding ';' at the end
                std::string formatSpecifiers = "", varNames = "", varTemp = "";
                for (int j = 0; j < setParserData[i + 1].length(); ++j) {
                    if (j + 1 < setParserData[i + 1].length() && setParserData[i + 1].substr(j, 2) == "${") {
                        int pos = setParserData[i + 1].find('}', j);
                        varTemp = setParserData[i + 1].substr(j + 2, pos - (j + 2));
                        std::vector<std::string> tokens = tokenizeExpression(varTemp);
                        bool done = false;
                        std::string formatType;
                        for (std::string var : tokens) {    // check if a declared variable is present or not
                            if (funcIndex >= 0 && functionScopeVarMappers[funcIndex].find(var) != functionScopeVarMappers[funcIndex].end()) {
                                formatType = functionScopeVarMappers[funcIndex].find(var)->second;
                                done = true;
                                break;

                            } else if (variableMapper.find(var) != variableMapper.end()) {
                                formatType = variableMapper.find(var)->second;   // store the variable type of the first variable encountered in the expression (can be improved)
                                done = true;
                                break;
                            }
                        }  
                        if (done) {
                            formatSpecifiers += formatType;
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
            if (i == 0) {
                if (dynamicNeeded) {    // dynamic memory allocation modules
                    writeFile << "#include \"modules/dynamicarray.h\"" << "\n";
                }
                for (std::string sig : functionHeaders) {   // function declarations
                    writeFile << sig << "\n";
                }
            }
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