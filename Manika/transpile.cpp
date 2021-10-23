#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<string>
std::unordered_map<std::string,std::string> datamapper;
std::vector<std::string> setperserData;
void refDataset()
{
    datamapper.insert({"htpl","#include<stdio.h>\n"});
    datamapper.insert({"main","void main(){\n"});
    datamapper.insert({"log","printf()"});
    datamapper.insert({"/","}"});
    datamapper.insert({"/log",";"});
    
}
void writeCode(){
    std::ofstream writeFile("output.c");
    for(int i=0;i<setperserData.size()-1;i++){
        writeFile<<setperserData[i]<<"\n";
    }
}


std::string removefontSpace(std::string getData)
{
    for( int i=0;i<getData.length();i++){
        if(getData[i]!=' '){
            getData=getData.substr(i,getData.length()-i);
            break;
        }

    }
    return getData;
}
std::string stripBrace(std::string getData){
    return getData.substr(1,getData.find('>')-1);
}
void printParser(){
    std::string string_const;
    for(int i=0;i<setperserData.size()-1;i++){
      if(setperserData[i]=="printf()"){
        string_const=setperserData[i].substr(0,7)+'"'+setperserData[i+1]+'"'+")"+setperserData[i+2];
        setperserData[i]=string_const;
        setperserData.erase(std::next(setperserData.begin(),i+1),std::next(setperserData.begin(),i+3));
      }
    }
}
void Parser(std::string getData){
    getData=removefontSpace(getData);
    if(getData[0]=='<')
{
        std::string getTag = stripBrace(getData);
        std::string helper_string = getData.substr(getData.find('>') + 1, getData.length() - (getData.find('>') + 1));
        if (helper_string.find('<') != std::string::npos && helper_string[helper_string.find('<') + 1] == '/')
        {

            if (datamapper.find(getTag) != datamapper.end())
            {
                setperserData.push_back(datamapper.find(getTag)->second);
            }
            std::string toRemove = helper_string.substr(helper_string.find('/') - 1);
            helper_string.erase(helper_string.find(toRemove), toRemove.length());
            setperserData.push_back(helper_string);
            std::string getEndTag = stripBrace(getData.substr(getData.substr(1).find('<') + 1));
            if (getEndTag.substr(1, 3) == "log")
            {
                setperserData.push_back(datamapper.find("/log")->second);
            }
            else
            {
                setperserData.push_back(datamapper.find("/")->second);
            }
        }
        else if (getData[1] != '/')
        {
            std::string getTag = stripBrace(getData);
            if (datamapper.find(getTag) != datamapper.end())
            {
                setperserData.push_back(datamapper.find(getTag)->second);
            }
        }
        else
        {
            if (getData.substr(2, 3) == "log")
                setperserData.push_back(datamapper.find("/log")->second);
            else
                setperserData.push_back(datamapper.find("/")->second);
        }
    } else{
        if(setperserData[setperserData.size()-1] == "printf()"){
            setperserData.push_back(getData);
        }else{
            setperserData[setperserData.size()-1]+=(" "+getData);
        }      
    }
    
}

int main(int argc, const char *argv[])
{
    refDataset();
    std::string getSyntax;
    std::ifstream readData(argv[1]);
    while(getline(readData,getSyntax))
    {
        Parser(getSyntax);
    }
    printParser();
    writeCode();
}