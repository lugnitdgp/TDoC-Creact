#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
std::unordered_map<std::string, std::string> dataMapper;
std::unordered_map<std::string, std::string> variableMapper;
std::vector<std::string> setParserData;

void refDataset()
{
    dataMapper.insert({"htpl" , "#include<stdio.h>\n"});
    dataMapper.insert({"main" , "void main(){\n"});
    dataMapper.insert({"log" , "printf()"});
    dataMapper.insert({"/" , "}"});
    dataMapper.insert({"/log" , ";"});
    dataMapper.insert({"in" , "int"});
    dataMapper.insert({"ch" , "char"});
    dataMapper.insert({"int" , "%d"});
    dataMapper.insert({"char" , "%c"});
    dataMapper.insert({"take" , "scanf()"});
}


void writeCode (){
    std::ofstream writeFile("output.c");
    for (int i=0;i<setParserData.size()-1; i++)
    { writeFile<<setParserData[i]<<"\n";
    }
}


/*std::string removeFrontSpaces (std::string getData)
{
    for (int i = 0; i < getData.length(); i++) {
        if (getData[i] != ' ')
        {
            getData = getData.substr(i, getData.length() - i); 
            break;
        }
    }
    return getData;
}
*/

std::string SpaceDebug(std::string getData)
{
    long long int ptr1=0,ptr2=getData.length()-1;
    while(getData[ptr1]==' ' || getData[ptr2]==' ')
    {
        if(getData[ptr1]==' ' )
            ptr1++;
        if(getData[ptr2]==' ' )
            ptr2--;
    }
    return getData.substr(ptr1,ptr2-ptr1+1);
}

std::string stripBraces(std::string getData)
{
    return getData.substr(1, getData.find('>') - 1);
}


void printParser() {
    std::string string_const="";
    for(int i=0;i<setParserData.size();i++){
        if(setParserData[i]=="printf()"){
            int pos;
            std::string formatSpecifiers = "", varNames = "", varName="";
            for(int j=0;j<setParserData[i+1].size();j++)
            {
                if(setParserData[i+1].substr(j,2) == "${")
                {
                    pos = setParserData[i+1].substr(j).find('}') + j;
                    varName = setParserData[i+1].substr(j+2, pos - (j+2));
                    if (variableMapper.find(varName) != variableMapper.end())
                    {
                        formatSpecifiers+=variableMapper.find(varName)->second;
                        varNames+=((varNames=="")?"":",") + varName;
                    }
                    else
                    {
                        formatSpecifiers+=setParserData[i+1].substr(j,pos-j+1);
                    }
                    j=pos;
                }
                else
                {
                    formatSpecifiers+=setParserData[i+1][j];
                }
            }
            string_const = setParserData[i].substr(0,7) +'"'+ setParserData[i+1] + '"' + ((varNames=="")?"":",") + varNames + ")" + setParserData[i + 2];
            setParserData[i] = string_const;
            setParserData.erase(std::next(setParserData.begin(),i+1), std::next (setParserData.begin(),i+3));
        }
        else if(setParserData[i]=="scanf")
        {
            string_const = setParserData[i].substr(0,6) + '"' + variableMapper.find(setParserData[i+1])->second + '"' + ",&" + setParserData[i+1] + ')' + ';';
            setParserData[i] = string_const;
            setParserData.erase((setParserData.begin()+i+1));
        }
    } 
}


std::string arrangeDebugger(std::string getData)
{
    std::string newGetData=""; 
    for (int i = 0; i < getData.length(); i++)
    {
        if(getData[i]=='=' || getData[i]==',')
        { 
            newGetData+=' ';
            newGetData+=getData[i];
            newGetData+=' ';
        } else {
            newGetData+=getData[i];
        }
    }
    return newGetData;
}


void IOparser(std::string getData)
{
    getData = arrangeDebugger(getData);
    std::vector<std::string> tokenContainer; 
    std::string ins_string="";
    for (int i = 0; i < getData.length(); i++)
    {
        if (getData[i] != ' ')
        {
            ins_string += getData[i];
        }
        else
        {
            if (ins_string.length() != 0)
            {
                tokenContainer.push_back(ins_string); 
                ins_string = "";
            }
        }
    }
    for (int i = 1; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i]=="a" ||tokenContainer[i] == ",")
        {
            variableMapper.insert({tokenContainer[i-1], dataMapper.find((dataMapper.find(tokenContainer[0])->second))->second});
        }
    }
}


void Parser(std::string getData){
    getData=SpaceDebug(getData);
    if(getData[0] == '<'){
        if(getData[getData.length()-1]=='>' && getData[getData.length()-2]=='/')
        {
            if(getData[getData.length()-3]=='/')
            {
                getData = getData.substr(1, getData.length() - 4);
                getData= SpaceDebug(getData)+" ";
                std::vector<std::string> tmp;
                std::string stf;
                for (int i = 0; i < getData.length(); i++)
                {
                    if (getData[i] != ' ')
                    {
                        stf = stf + getData[i];
                    }
                    else
                    {
                        if(stf.length() != 0)
                            tmp.push_back(stf);
                            stf = "";
                    }
                }
                if(tmp[0]=="take"){
                    setParserData.push_back(dataMapper.find(tmp[0])->second);
                    setParserData.push_back(tmp[1]);
                }
            }
            else
            {
                getData=getData.substr(1,getData.length()-3);
                getData=SpaceDebug(getData);
                setParserData.push_back(dataMapper.find(getData.substr(0,2))->second + getData.substr(2, getData.length() - 2) + ";");  
                IOparser(getData);
            }
        }
        else if (getData[1] != '/')
        {
        std::string getTag = stripBraces(getData);
        getTag=SpaceDebug(getTag);
        if (dataMapper.find(getTag) != dataMapper.end()) 
        { 
            setParserData.push_back(dataMapper.find(getTag)->second);
        }
        } else {
            if (getData.substr(2, 3) == "log")
                setParserData.push_back (dataMapper.find("/log")->second);
            else
                setParserData.push_back (dataMapper.find("/") ->second);
        }
    }else{
        if (setParserData[setParserData.size() - 1] == "printf()")
            setParserData.push_back (getData);
        else 
            setParserData[setParserData.size()-1]+=(" "+getData);
    }
}


int main(int argc, char const *argv[])
{
    refDataset();
    std::string getsyntax;
    std::ifstream readData(argv[1]);
    while(getline(readData, getsyntax))
    {
        Parser(getsyntax);
    }
    for(auto it:setParserData){
        std::cout<<it<<"\n";
    }
    printParser();
    writeCode();
}

