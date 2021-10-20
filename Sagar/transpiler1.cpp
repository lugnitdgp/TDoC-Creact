#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> dataMapper;
std::unordered_map<std::string,std::string> variableMapper;
std::vector<std::string> setParserData;
void refDataset()
{
    dataMapper.insert({"htpl", "#include<stdio.h>\n"});
    dataMapper.insert({"main", "void main(){\n"});
    dataMapper.insert({"log", "printf()"});
    dataMapper.insert({"/log", ";"});
    dataMapper.insert({"/", "}"});
    dataMapper.insert({"/log","."});
    dataMapper.insert({"in","int"});
    dataMapper.insert({"ch","char"});
    dataMapper.insert({"int","%d"});
    dataMapper.insert({"char","%c"});
    dataMapper.insert({"take","scanf()"});
}



void writeCode(){
    std::ofstream writeFile("output.c");
    for(int i=0;i<setParserData.size()-1;i++){
        writeFile<<setParserData[i]<<"\n";
    }
}



std::string stringSpaceDebug(std::string getData){
    int a=0,b=getData.size()-1;
    while(getData[a]==' ')
    a++;
    while(getData[b]==' ')
    b--;

    return getData.substr(a,b-a+1);
}


std::string stripBraces(std::string getData){
    return getData.substr(1,getData.find('>')-1);
}

void printParser()
{
    std::string string_const = "";
    for (int i = 0; i < setParserData.size(); i++)
    {
        if (setParserData[i] == "printf()")
        {
            int pos;
            std::string formatSpeifiers="",varNames="",varName="";
            for(int j=0;j<setParserData[i+1].size();j++){
                if(setParserData[i+1].substr(j,2)=="${"){
                    pos=setParserData[i+1].substr(j).find('}')+j;
                    varName = setParserData[i+1].substr(j+2,pos-(j+2));
                    if(variableMapper.find(varName)!=variableMapper.end()){
                        formatSpeifiers+=variableMapper.find(varName)->second;
                        varNames+=((varNames=="")?"":",") + varName;
                    }
                    else{
                        formatSpeifiers+=setParserData[i+1].substr(j,pos-j+1);
                    }
                    j=pos;
                }
                else{
                    formatSpeifiers+=setParserData[i+1][j];
                }
            }
            string_const = setParserData[i].substr(0,7) + '"' + formatSpeifiers + '"' + ((varNames=="")?"":",") +varNames + ")" + setParserData[i + 2];
            setParserData[i] = string_const;
            setParserData.erase(std::next(setParserData.begin(),i+1),std::next(setParserData.begin(),i+3));
        }
        else if(setParserData[i]=="scanf()"){
            string_const=setParserData[i].substr(0,6) + '"' + variableMapper.find(setParserData[i+1])->second + '"' + ",&" + setParserData[i+1] + ')' + ';' ;
            setParserData[i]=string_const;
            setParserData.erase(setParserData.begin()+i+1);
        }
    }
}


std::string arrangeDebugger(std::string getData){
    std::string newGetData = "";
    for(int i=0;i<getData.length();i++){
        if(getData[i]=='=' || getData[i]==','){
            newGetData+= ' ';
            newGetData+=getData[i];
            newGetData+=' ';
        }
        else
        newGetData += getData[i];
    }
    return newGetData;
}

void IOParser(std::string getData){
    getData = arrangeDebugger(getData);
    std::vector<std::string>tokenContainer;
    std::string ins_string ="";
    for(int i=0;i<getData.length()-1;i++){
        if(getData[i]!=' '){
            ins_string+=getData[i];
        }
        else{
            if(ins_string.length()!=0){
                tokenContainer.push_back(ins_string);
                ins_string="";
            }
        }
    }
    for(int i=1;i<tokenContainer.size();i++){
        if(tokenContainer[i]== "=" || tokenContainer[i]==","){
            variableMapper.insert({tokenContainer[i-1],dataMapper.find((dataMapper.find(tokenContainer[0])->second))->second});
        }
    }
}

void Parser(std::string getData)
{
    //refDataset();
    getData = stringSpaceDebug(getData);
    if (getData[0] == '<')
    {   
        if(getData[getData.length()-1]=='>' && getData[getData.length()-2]=='/'){
            
            if(getData[getData.length()-3]=='/'){
                getData=getData.substr(1,getData.length()-4);
                getData=stringSpaceDebug(getData) + " ";
                std::vector<std::string> tmp;
                std::string stf;
                for(int i=0;i<getData.length();i++){
                    if(getData[i]!=' ')
                    stf+=getData[i];
                    else
                    {
                        if(stf.length()!=0)
                        {
                            tmp.push_back(stf);
                            stf="";
                        }
                    }
                }
                if(tmp[0]=="take"){
                    setParserData.push_back(dataMapper.find(tmp[0])->second);
                    setParserData.push_back(tmp[1]);
                }
            }
            else{
                getData = getData.substr(1,getData.length()-3);
                getData=stringSpaceDebug(getData);
                setParserData.push_back(dataMapper.find(getData.substr(0,2))->second + getData.substr(2,getData.length()-2) + ';');
                IOParser(getData);
            }
        }
        else if (getData[1] != '/')
        {
            std::string getTag = stripBraces(getData);
            getTag = stringSpaceDebug(getTag);
            if (dataMapper.find(getTag) != dataMapper.end())
            {
                if(getTag=="log" && getData.substr(getData.length()-5,4)=="/log"){
                    setParserData.push_back(dataMapper.find(getTag)->second);
                    setParserData.push_back(getData.substr(getData.find('>')+1,getData.substr(getData.find('>')+1,getData.length()-(getData.find('>')+1)).find('<')));
                    setParserData.push_back(";");
                } else {
                    setParserData.push_back(dataMapper.find(getTag)->second);
                }
            }
        }
        else
        {
            if (getData.substr(2, 3) == "log")
                setParserData.push_back(dataMapper.find("/log")->second);
            else
                setParserData.push_back(dataMapper.find("/")->second);
        }
    }
    else
    {
        if (setParserData[setParserData.size() - 1] == "printf()")
            setParserData.push_back(getData);
        else
            setParserData[setParserData.size() - 1] += (" " + getData);
    }
}

int main(int argc, char const *argv[]){
    refDataset();
    std::string getSyntax;
    std::ifstream readData(argv[1]);
    while(getline(readData,getSyntax)){
        Parser(getSyntax);
    }
    printParser();
    writeCode();
    return 0;
}