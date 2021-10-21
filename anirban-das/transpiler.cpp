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
        if(syntax[i]==',' || syntax[i]=='='){
            arranged += ' ';
            arranged += syntax[i];
            arranged += ' ';
        } else{
            arranged += syntax[i];
        }
    }
    return arranged;
}

void parseIO(std::string syntax){
    syntax = arrangeIOSyntax(syntax);
    std::vector<std::string> tokens;
    std::string s;

    for(int i=0; i<syntax.length(); i++){
        if(syntax[i] != ' '){
            s += syntax[i];
        } else {
            tokens.push_back(s);
            s = "";
        }
    }

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
    // syntax = <take a/>

    // for syntax which are symbols
    if(syntax[0] == '<'){

        // for opening symbols
        if(syntax[1] != '/'){

            // for 'self closing' variable declaration tags
            if(syntax[syntax.length()-1] == '>' && syntax[syntax.length()-2] == '/'){

                if(syntax[syntax.length()-3] == '/'){
                    // std:/:cout<<syntax<<"/n";
                    syntax = syntax.substr(1, syntax.length()-4);
                    syntax = trimFrontSpaces(syntax) + " ";

                    std::vector<std::string> tokens;
                    std::string s;

                    for(int i=0; i<syntax.length(); i++){
                        if(syntax[i] != ' '){
                            s = s + syntax[i];
                        } else{
                            if(s.length() != 0)
                                tokens.push_back(s);
                            s = "";
                        }
                    }

                    if(tokens[0] == "take"){
                        parsedData.push_back(symbolTable.find(tokens[0])->second);
                        parsedData.push_back(tokens[1]);
                    }

                    std::cout<<syntax<<"/n";

                } else{
                    // <in c=9/>
                    syntax = syntax.substr(1, syntax.length()-3);
                    syntax = trimFrontSpaces(syntax);
                    parsedData.push_back(symbolTable.find(syntax.substr(0,2))->second + syntax.substr(2, syntax.length()-2) + ";");
                    parseIO(syntax);
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
    } else{ // for syntax which are printable statements

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
            for(int j=0; j<parsedData[i+1].length(); j++){
                if(parsedData[i+1].substr(j,2) == "${"){ // checks if a variable is to be printed
                    int pos = parsedData[i+1].substr(j).find('}') + j;
                    std::string name = parsedData[i+1].substr(j+2, pos-(j+2));
                    if(typeTable.find(name) != typeTable.end()){ // checks for validity of variable name
                        specifiers += typeTable.find(name)->second;
                        varNames += (varNames == "" ? "" : ",") + name;
                    } else{ // else throws error
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
            element = parsedData[i].substr(0,6) + '"' + typeTable.find(parsedData[i+1])->second + '"' + ",&" + parsedData[i+1] + ");";
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