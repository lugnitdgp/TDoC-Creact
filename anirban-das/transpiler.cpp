// headers
#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<string>

// global collections
std::unordered_map<std::string, std::string> symbolTable;
std::unordered_map<std::string, std::string> typeTable;
std::vector<std::string> parsedData;
std::vector<std::string> functionHeaders;

// global variables
std::string doWhileExpression = "";
bool isDoWhilePresent = false;

// to create table including {key, value} pairs to define the language
void makeTable(){
    symbolTable.insert({"htpl", "#include<stdio.h>\n"});
    symbolTable.insert({"main", "void main(){\n"});
    symbolTable.insert({"log", "printf()"});
    symbolTable.insert({"/", "}"});
    symbolTable.insert({"/log", ";"});
    symbolTable.insert({"in", "int"});
    symbolTable.insert({"ch", "char"});
    symbolTable.insert({"db", "double"});
    symbolTable.insert({"take", "scanf()"});
    symbolTable.insert({"int", "%d"});
    symbolTable.insert({"char", "%c"});
    symbolTable.insert({"double", "%lf"});
}

// to remove the indentation spaces
std::string trimFrontSpaces(std::string syntax){
    long long int ptr1 = 0, ptr2 = syntax.length()-1;
    while(syntax[ptr1]==' ' || syntax[ptr2]==' '){
        if(syntax[ptr1]==' ')
            ptr1++;
        if(syntax[ptr2]==' ')
            ptr2--;
    }
    
    return syntax.substr(ptr1, ptr2-ptr1+1);
}

std::string arrangeIOSyntax(std::string syntax){
    std::string arranged = "";
    for(int i=0; i<syntax.length(); i++){
        if(syntax[i]==',' || syntax[i]=='=' || syntax[i] == ' '){
            arranged += ' ';
            arranged += syntax[i];
            arranged += ' ';
        } else if(syntax[i] != ' '){
            arranged += syntax[i];
        }
    }
    return arranged;
}

std::vector<std::string> arrangeExpessionsInLog(std::string syntax){
    std::vector<std::string> tokens;
    std::string str="", temp="";

    for(int i=0; i<syntax.length(); i++){
        if(syntax[i] == '+' || syntax[i] == '/' || syntax[i] == '*' || syntax[i] == '-' || syntax[i] == '^' || syntax[i] == '%' || syntax[i] == ' '){
            str += ' ';
            str += syntax[i];
            str += ' ';
        }
        else{
            str += syntax[i];
        }
    }
    str += ' ';
    for(int i=0; i<str.length(); i++){
        if(str[i] != ' '){
            temp += str[i];
        }
        else{
            if(temp != "")
                tokens.push_back(temp);
            temp = "";
        }
    }
    return tokens;
}

void parseIO(std::string syntax){
    syntax = arrangeIOSyntax(syntax) + " ";
    std::vector<std::string> tokens;
    std::string s;

    for(int i=0; i<syntax.length(); i++){
        if(syntax[i] != ' '){
            s += syntax[i];
        } else {
            if(s != "")
                tokens.push_back(s);
            s = "";
        }
    }
    tokens.push_back(",");
    for(int i=0; i<tokens.size(); i++){
        if(tokens[i] == "=" || tokens[i] == ","){
            typeTable.insert({tokens[i-1], symbolTable.find(symbolTable.find(tokens[0])->second)->second});
        }
    }
}

// to get the processed "C" syntax from "CReact"
void parseSyntax(std::string syntax){
    syntax = trimFrontSpaces(syntax);
    // std::cout<<syntax<<"\n";

    // for syntax which are symbols
    if(syntax[0] == '<' || syntax[0] == '?' || syntax[0] == '#'){

        // for opening symbols
        if(syntax[1] != '/'){

            // for 'self closing' variable declaration tags
            if(syntax[syntax.length()-1] == '>' && syntax[syntax.length()-2] == '/'){

                // function call statements
                if(syntax.find('f') != std::string::npos && syntax[syntax.find('f')+1] == 'x'){
                    syntax = syntax.substr(1, syntax.length()-3);
                    syntax = trimFrontSpaces(syntax);

                    syntax = syntax.substr(3, syntax.length()-3) + ";";
                    parsedData.push_back(syntax);
                }

                // return statements
                else if(trimFrontSpaces(syntax.substr(1, syntax.length()-3)).substr(0, 5) == "throw"){
                    syntax = syntax.substr(1, syntax.length()-3);
                    syntax = trimFrontSpaces(syntax);
                    syntax = "return" + syntax.substr(5, syntax.length()-5) + ";";
                    parsedData.push_back(syntax);
                }

                else if(syntax[syntax.length()-3] == '/'){
                    // <take a,b //>
                    syntax = syntax.substr(1, syntax.length()-4);
                    syntax = trimFrontSpaces(syntax) + " ";

                    std::vector<std::string> tokens;
                    std::string s;

                    for(int i=0; i<syntax.length(); i++){
                        if(syntax[i] != ' ' && syntax[i] != ','){
                            s = s + syntax[i];
                        } else{
                            if(s.length() != 0)
                                tokens.push_back(s);
                            s = "";
                        }
                    }

                    if(tokens[0] == "take"){
                        std::string str="";
                        parsedData.push_back(symbolTable.find(tokens[0])->second);
                        for(int i=1; i<tokens.size(); i++){
                            str += tokens[i] + " ";
                        }
                        parsedData.push_back(str);
                    }   

                } else{
                    // <in c=9/>
                    syntax = syntax.substr(1, syntax.length()-3);
                    syntax = trimFrontSpaces(syntax);
                    parsedData.push_back(symbolTable.find(syntax.substr(0,2))->second + syntax.substr(2, syntax.length()-2) + ";");
                    parseIO(syntax);
                }
            }

            // function definitions
            else if(syntax.find('f') != std::string::npos && syntax[syntax.find('f')+1] == 'x'){
                syntax = syntax.substr(1, syntax.length()-1);
                syntax = trimFrontSpaces(syntax);

                std::string str="", temp="";
                for(int i=0; i<syntax.length(); i++){
                    if(syntax[i] == '(' || syntax[i] == ')' || syntax[i] == ','){
                        str += ' ';
                        str += syntax[i];
                        str += ' ';
                    } else{
                        str += syntax[i];
                    }
                }
                syntax = str, str="";
                for(int i=0; i<syntax.length(); i++){
                    if(syntax[i] != ' '){
                        str += syntax[i];
                    } else {
                        if(str == "in" || str == "ch" || str == "db"){
                            temp += symbolTable.find(str)->second + " ";
                        } else if(str == "void"){
                            temp += str + " ";
                        } else {
                            temp += str;
                        }
                        str = "";
                    }
                }
                functionHeaders.push_back(temp.substr(2, temp.length()-2) + ";");
                temp += "{";
                parsedData.push_back(temp.substr(2, temp.length()-2));
            }

            // integrating arithmatic expression
            else if(syntax[1] == '%'){
                std::vector<std::string> tokens;
                std::string str="";
                for(int i=2; i<syntax.length(); i++){
                    if(syntax[i] == '%')
                        break;
                    str += syntax[i];
                }
                str += ';';
                parsedData.push_back(str);
            }

            // conditional statement integration
            else if(syntax.substr(0, 2) == "<?" || syntax == "?>"){
                if(syntax != "?>"){
                    syntax = syntax.substr(2, syntax.length()-2);
                    syntax = trimFrontSpaces(syntax);

                    if(syntax.substr(0,2) == "if"){
                        syntax = syntax + '{';
                        parsedData.push_back(syntax);
                    } else if(syntax.substr(0,4) == "elif"){
                        syntax = "else if" + syntax.substr(4, syntax.length()-4) + '{';
                        parsedData.push_back(syntax);
                    } else if(syntax.substr(0,4) == "else"){
                        syntax = syntax + '{';
                        parsedData.push_back(syntax);
                    }
                } else{
                    parsedData.push_back("}");
                }
            }

            // loop statements
            else if(syntax.substr(0,2) == "<#" || syntax == "#>"){
                
                // do while loop
                if(trimFrontSpaces(syntax.substr(2, syntax.length()-2)).substr(0,2) == "dw"){
                    syntax = syntax.substr(2, syntax.length()-2);
                    syntax = trimFrontSpaces(syntax);
                    
                    doWhileExpression = syntax.substr(2, syntax.length()-2);
                    isDoWhilePresent = true;

                    syntax = "do{";
                    parsedData.push_back(syntax);
                } else{
                    if(syntax != "#>"){ // for and while loop
                        syntax = syntax.substr(2, syntax.length()-2);
                        syntax = trimFrontSpaces(syntax);
                        if(syntax[0] == 'f'){
                            for(int i=2; i<syntax.length(); i++){
                                if(syntax[i] == '@')
                                    syntax[i] = ',';
                                if(syntax[i] == ',')
                                    syntax[i] = ';';
                            }
                            syntax = "for" + syntax.substr(1, syntax.length()-1) + "{";
                            parsedData.push_back(syntax);
                        } else if(syntax[0] == 'w'){
                            syntax = "while" + syntax.substr(1, syntax.length() - 1) + "{";
                            parsedData.push_back(syntax);
                        }
                    } else { // closing braces for loops
                        if(isDoWhilePresent){
                            parsedData.push_back("}while" + doWhileExpression + ";");
                            isDoWhilePresent = false;
                        } else
                            parsedData.push_back("}");
                    }
                }
            }

            // if logs are inline
            else if(syntax.substr(1,3) == "log" && syntax.substr(syntax.length()-4, 3) == "log"){
                parsedData.push_back(symbolTable.find("log")->second);
                syntax = syntax.substr(5, syntax.length()-5);
                int index = syntax.find_first_of('<');
                std::string textBlock = syntax.substr(0, index);
                parsedData.push_back(textBlock);
                parsedData.push_back(symbolTable.find("/log")->second);

                syntax = syntax.substr(index+6, syntax.length()-index-6);

                // recursion used to handle multiple logs in same line
                if(syntax != "")
                    parseSyntax(syntax);

            } else{ // other opening symbols
                std::string symbol = syntax.substr(1, syntax.find('>') - 1);
                if(symbolTable.find(symbol) != symbolTable.end()){
                    parsedData.push_back(symbolTable.find(symbol)->second);
                } else{
                    std::cout<<"Non-Recognised Symbol <" + symbol + ">...terminating script\n";
                    exit(0);
                }
            }
        } else{ // for closing symbols

            // for print statement
            if(syntax.substr(2,3) == "log"){
                parsedData.push_back(symbolTable.find("/log")->second);
            } else{ // for other statements 
                parsedData.push_back(symbolTable.find("/")->second);
            }
        }
    } else if(syntax[0] != '?' || syntax[0] != '#'){ // for syntax which are printable statements

        // if previous element is print statement, push the printable statement as it is
        if(parsedData[parsedData.size()-1] == "printf()"){
            parsedData.push_back(syntax);
        } else{ // else append all the printable statements together
            parsedData[parsedData.size()-1] += " " + syntax; 
        }
    }
}

// to print the parsed data into "C" file
void processPrintStatements(){
    std::string element;
    for(int i=0; i<parsedData.size(); i++){

        // for 'log' statements
        if(parsedData[i] == "printf()"){
            std::string specifiers="", varNames="";

            // processing variables initialised during run time before printing,
            // throwing error for invalid variable names
            for(int j=0; j<parsedData[i+1].size(); j++){
                if(parsedData[i+1].substr(j,2) == "${"){ // checks if a variable is to be printed
                    int pos = parsedData[i+1].substr(j).find('}') + j;
                    std::string name = parsedData[i+1].substr(j+2, pos-(j+2));
                    
                    std::vector<std::string> tokens = arrangeExpessionsInLog(name);
                    int isExpression = 1;

                    for(int k=0; k<tokens.size(); k++){
                        if(typeTable.find(tokens[k]) != typeTable.end()){ // checks for validity of variable name
                            specifiers += typeTable.find(tokens[k])->second;
                            varNames += (varNames == "" ? "" : ",") + name;
                            isExpression = 0;
                            break;
                        }
                    }
                    if(isExpression == 1){ // else throws error
                        std::cout<<"\n";
                        std::cout<<"Non-Recognised Symbol '" + name + "'...terminating script\n";
                        exit(0);
                    }
                    j = pos;
                } else{ // prints non-variables
                    specifiers += parsedData[i+1][j];
                }
            }

            element = parsedData[i].substr(0,7) + '"' + specifiers + '"' + (varNames == "" ? "" : ",") + varNames + ")" + parsedData[i+2];
            parsedData[i] = element;
            parsedData.erase(std::next(parsedData.begin(), i+1), std::next(parsedData.begin(), i+3));
        } else if(parsedData[i] == "scanf()"){ // for 'take' statements
            std::string specifiers="", varNames="", name="";

            for(int j=0; j<parsedData[i+1].size(); j++){
                if(parsedData[i+1][j] != ' '){
                    name += parsedData[i+1][j];
                } else{
                    if(typeTable.find(name) == typeTable.end()){
                        std::cout<<"s\n";
                        std::cout<<"Non-Recognised Symbol '" + name + "'...terminating script\n";
                        exit(0);
                    }
                    specifiers += typeTable.find(name)->second + " ";
                    name = "&" + name;
                    varNames += ((varNames == "") ? "" : ",") + name;
                    name = "";
                }
            }            
            element = parsedData[i].substr(0,6) + '"' + specifiers.substr(0, specifiers.length() - 1) + '"' + "," + varNames + ");";
            parsedData[i] = element;
            parsedData.erase((parsedData.begin() + i+1));
        }
    }
}

// to write the "C" code in a file
void writeCodeToFile(int flag){
    std::ofstream writeFile("output.c");
    if(!flag){
        for(int i=0; i<parsedData.size()-1; i++){
            writeFile << parsedData[i] << "\n";

            // writing the headers before main starts executing
            if(i==0){
                for(int j=0; j<functionHeaders.size(); j++){
                    writeFile << functionHeaders[j] << "\n";
                }
            }
        }
    } 
}

// main
int main(int argc, char const *argv[]){
    makeTable();
    std::string currentSyntax;

    // read input CReact file
    std::ifstream readFile(argv[1]);

    // read each line and process the code
    while(getline(readFile, currentSyntax)){
        parseSyntax(currentSyntax);
    }

    // arranging the syntax and writting to file
    processPrintStatements();
    writeCodeToFile(0);
}