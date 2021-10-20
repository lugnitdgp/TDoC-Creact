#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<string>

std::unordered_map<std::string, std::string> symbolTable;
std::vector<std::string> parsedData;

// to create table including {key, value} pairs to define the language
void makeTable(){
    symbolTable.insert({"htpl", "#include<stdio.h>\n"});
    symbolTable.insert({"main", "void main(){\n"});
    symbolTable.insert({"log", "printf()"});
    symbolTable.insert({"/", "}"});
    symbolTable.insert({"/log", ";"});
}

// to remove the indentation spaces
std::string trimFrontSpaces(std::string syntax){
    for(int i=0; i<syntax.length(); i++){
        if(syntax[i] != ' '){
            syntax = syntax.substr(i, syntax.length()-i);
            break;
        }
    }
    return syntax;
}

// to remove the angle braces from syntax
std::string trimAngleBrackets(std::string syntax){
    return syntax.substr(1, syntax.find('>') - 1);
}

// to get the processed "C" syntax from "CReact"
void parseSyntax(std::string syntax){
    syntax = trimFrontSpaces(syntax);

    // for syntax which are symbols
    if(syntax[0] == '<'){

        // for opening symbols
        if(syntax[1] != '/'){

            // if logs are inline
            if(syntax.substr(1,3) == "log" && syntax.substr(syntax.length()-4, 3) == "log"){
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

            } else{
                std::string symbol = trimAngleBrackets(syntax);
                if(symbolTable.find(symbol) != symbolTable.end()){
                    parsedData.push_back(symbolTable.find(symbol)->second);
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
        if(parsedData[i] == "printf()"){
            element = parsedData[i].substr(0,7) + '"' + parsedData[i+1] + '"' + ")" + parsedData[i+2];
            parsedData[i] = element;
            parsedData.erase(std::next(parsedData.begin(), i+1), std::next(parsedData.begin(), i+3));
        } else {

        }
    }
}

// to write the "C" code in a file
void writeCodeToFile(){
    std::ofstream writeFile("output.c");
    for(int i=0; i<parsedData.size()-1; i++){
        writeFile << parsedData[i] << "\n";
    }
}

// main
int main(int argc, char const *argv[]){
    makeTable();
    std::string currentSyntax;

    // read input CReact file
    std::ifstream readFile(argv[1]);

    while(getline(readFile, currentSyntax)){
        parseSyntax(currentSyntax);
    }

    // for(int i=0; i<parsedData.size()-1; i++){
    //     std::cout<<parsedData[i]<<"\n";
    // }

    processPrintStatements();
    writeCodeToFile();
}