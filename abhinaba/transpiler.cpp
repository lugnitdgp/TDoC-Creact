#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<string>

//symo=bol table for mapping the creact commands to C commands
std::unordered_map<std::string, std::string> dataMapper;

//map for keeping the variable types
std::unordered_map<std::string, std::string>variableMapper;

//vector to store the individual statements converted to C
std::vector<std::string> setParserData;

//dataMapper reference dataset -- stores the necessary mappings
void refDataset(){
    dataMapper.insert({"htpl" , "#include<stdio.h>\n"});
    dataMapper.insert({"main" , "void main(){\n"});
    dataMapper.insert({"log", "printf()"});
    dataMapper.insert({"/","}"});
    dataMapper.insert({"/log", ";"});
    dataMapper.insert({"in", "int"});
    dataMapper.insert({"ch", "char"});
    dataMapper.insert({"int", "%d"});
    dataMapper.insert({"char", "%c"});
}

//write code to c file 
void writeCode(){
    std::ofstream writeFile("output.c");
    for(int i=0; i<setParserData.size()-1; i++){
        // std::cout<<setParserData[i]<<" ";
        writeFile<<setParserData[i]<<"\n";
    }
}


// a 2-pointer based approach to remove the spaces
std::string spaceDebug(std::string getData){
    long long p1 =  0, p2 = getData.length()-1;
    while(getData[p1] == ' ' || getData[p2]== ' '){
        if(getData[p1] == ' ')
            p1++;
        if(getData[p2] == ' ')
            p2--;
    }
    return getData.substr(p1, p2-p1+1);
}

//stripping the braces
std::string stripBraces(std::string getData){
    return getData.substr(1, getData.find('>')-1);
}

void printParser(){
    std::string string_const;
    for(int i=0;i<setParserData.size(); i++){

        if(setParserData[i]=="printf()"){
            string_const = setParserData[i].substr(0,7) + '"' + setParserData[i+1] + '"' + ")" + setParserData[i+2];
            setParserData[i] = string_const;
            setParserData.erase(std::next(setParserData.begin(), i+1), std::next(setParserData.begin(), i+3));
        }
    }
    
}

//function to separate out the spaces and commas in input statements
std::string separator(std::string getData){
    std::string sepgetData="";
    for(int i=0; i<getData.length(); i++){
        if(getData[i]=='=' || getData[i]==','){
            sepgetData+=' ';
            sepgetData+=getData[i];
            sepgetData+=' '; 
        }
        else{
            sepgetData+=getData[i];
        }
    }
    return sepgetData;
}

void IOparser(std::string getData){
    getData = separator(getData);
    std::vector<std::string> tokenContainer;
    std::string insert_str;
    //tokenize the entire declarative statement
    for(int i=0; i<getData.length(); i++){
        if(getData[i] != ' '){
            insert_str += getData[i];
        }
        else{
            if(insert_str.length() !=0){
                tokenContainer.push_back(insert_str);
                insert_str = "";
            }

        }
    }

    //iterating the tokenContainer and mapping the variables with theier types
    for(int i=1; i<tokenContainer.size(); i++){
        if(tokenContainer[i] == "=" || tokenContainer[i]== ","){
            variableMapper.insert({tokenContainer[i-1], dataMapper.find((dataMapper.find(tokenContainer[0])->second))->second});
        }
    }

}


void Parser(std::string getData){
    getData = spaceDebug(getData);
    // std::cout<<getData<<" ";
    if(getData[0]=='<'){
        //for the variable declaration part
        if(getData[getData.length()-2]=='/' && getData[getData.length()-1]=='>'){
            getData = getData.substr(1, getData.length()-3);
            getData = spaceDebug(getData);
            //inserting the entire delcaration fo the variable
            setParserData.push_back(dataMapper.find(getData.substr(0,2))->second + getData.substr(2, getData.length()-2) + ";" );
            IOparser(getData);
        }    

        else if(getData[1] !='/'){
            std::string getTag = stripBraces(getData);
            getTag =  spaceDebug(getTag);
            if(dataMapper.find(getTag) != dataMapper.end()) {

                if(getTag == "log" && getData.substr(getData.length() - 5, 4)=="/log"){
                    setParserData.push_back(dataMapper.find(getTag)->second);
                    long long length = getData.substr(getData.find('>')+1, getData.length()-getData.find('>')).length()-6;
                    setParserData.push_back( getData.substr(getData.find('>')+1, length));
                    setParserData.push_back(";");
                }
                else 
                    setParserData.push_back(dataMapper.find(getTag)->second);
            }

        } else {
            if(getData.substr(2, 3)=="log")
                setParserData.push_back(dataMapper.find("/log")->second);
            else
                setParserData.push_back(dataMapper.find("/")->second);
        }
    } else {
        if(setParserData[setParserData.size()-1]=="printf()")
            setParserData.push_back(getData);
        else
            setParserData[setParserData.size()-1] += (" "+getData);
    }
}

int main(int argc, char const *argv[]){
    refDataset();
    std::string getSyntax;
    std::ifstream readData(argv[1]);
    while(getline(readData, getSyntax)){
        Parser(getSyntax);
    }
    printParser();
    writeCode();
    // for(int i=0; i<setParserData.size(); i++)
    //     std::cout<<setParserData[i]<<" ";
    return 0;
}