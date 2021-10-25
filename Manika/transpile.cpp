#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
std::unordered_map<std::string, std::string> varibaleMapper;
std::unordered_map<std::string, std::string> datamapper;
std::vector<std::string> setperserData;
std::vector<std::string> functionHeader;
void refDataset()
{
    datamapper.insert({"htpl", "#include<stdio.h>\n"});
    datamapper.insert({"main", "void main(){\n"});
    datamapper.insert({"log", "printf()"});
    datamapper.insert({"/", "}"});
    datamapper.insert({"/log", ";"});
    datamapper.insert({"in", "int"});
    datamapper.insert({"ch", "char"});
    datamapper.insert({"fl", "float"});
    datamapper.insert({"int", "%d"});
    datamapper.insert({"char", "%d"});
    datamapper.insert({"take", "scanf()"});
    datamapper.insert({"float", "%f"});
}
void writeCode()
{
    std::ofstream writeFile("output.c");
    for (int i = 0; i < setperserData.size() - 1; i++)
    {
        writeFile << setperserData[i] << "\n";
        if (i == 0)
        {
            for (int k = 0; k < functionHeader.size(); k++)
            {
                writeFile << functionHeader[k] << "\n";
            }
        }
    }
}
std::string spaceDebug(std::string getData)
{
    long long int ptr1 = 0, ptr2 = getData.length() - 1;
    while (getData[ptr1] == ' ' || getData[ptr2] == ' ')
    {
        if (getData[ptr1] == ' ')
        {
            ptr1++;
        }
        if (getData[ptr2] == ' ')
        {
            ptr2--;
        }
    }
    return getData.substr(ptr1, ptr2 - ptr1 + 1);
}
std::string stripBrace(std::string getData)
{
    return getData.substr(1, getData.find('>') - 1);
}
std::vector<std::string> fxEval(std::string st)
{
    std::vector<std::string> store;
    std::string newgetData = "";
    for (int i = 0; i < st.length(); i++)
    {
        if (st[i] == '+' || st[i] == '-' || st[i] == '*' || st[i] == '/' || st[i] == '%' || st[i] == '^')
        {
            newgetData += ' ';
            newgetData += st[i];
            newgetData += ' ';
        }
        else
        {
            newgetData += st[i];
        }
    }
    newgetData += ' ';
    std::string tmp = "";
    for (int i = 0; i < newgetData.length(); i++)
    {
        if (newgetData[i] != ' ')
        {
            tmp += newgetData[i];
        }
        else
        {
            store.push_back(tmp);
            tmp = "";
        }
    }
    /*for(auto it:varibaleMapper){
        std::cout<<it.first<<" "<<it.second<<"\n";
    }
    for(auto it:store){
        std::cout<<it<<"\n";
    }*/
    return store;
}

void printParser()
{
    std::string string_const;
    for (int i = 0; i < setperserData.size(); i++)
    {
        if (setperserData[i] == "printf()")
        {
            int pos;
            std::string formatSpecifiers = "", varNames = "", varName = "";
            std::vector<std::string> exval;
            for (int j = 0; j < setperserData[i + 1].size(); j++)
            {
                if (setperserData[i + 1].substr(j,2) == "${")
                {
                    pos = setperserData[i + 1].substr(j).find('}') + j;
                    varName = setperserData[i + 1].substr(j + 2, pos - (j + 2));
                    exval = fxEval(varName);
                    bool check = 1;
                    //for(auto it:exval) std::cout<<it<<"\n";
                    for (int k = 0; k < exval.size(); k++)
                    {
                        if (varibaleMapper.find(exval[k]) != varibaleMapper.end())
                        {
                            formatSpecifiers += varibaleMapper.find(exval[k])->second;
                            varNames += ((varNames == "") ? "" : ",") + varName;
                            //std::cout<<formatSpecifiers<<"\n";
                            check = 0;
                            break;
                        }
                    }
                    if (check == 1)
                    {
                        {
                            formatSpecifiers += setperserData[i + 1].substr(j, pos - j + 1);
                        }
                        
                    }
                    j = pos;
                }
                else
                {
                    formatSpecifiers += setperserData[i + 1][j];
                }
            }

            string_const = setperserData[i].substr(0, 7) + '"' + formatSpecifiers + '"' + ((varNames == "") ? "" : ",") + varNames + ")" + setperserData[i + 2];
            setperserData[i] = string_const;
            setperserData.erase(std::next(setperserData.begin(), i + 1), std::next(setperserData.begin(), i + 3));
        }
        else if (setperserData[i] == "scanf()")
        {
            // std::unordered_map<std::string, std::string>::iterator itr;
            //string_const = setperserData[i].substr(0, 6) + '"' + varibaleMapper.find(setperserData[i + 1])->second + '"' + ",&" + setperserData[i + 1] + ')' + ';';
            std::string formatSpecifiers = "", varNames = "", varName = "";
            for (int j = 0; j < setperserData[i + 1].size(); j++)
            {
                if (setperserData[i + 1][j] != ' ')
                {
                    varName += setperserData[i + 1][j];
                }
                else
                {
                    formatSpecifiers += varibaleMapper.find(varName)->second + " ";
                    varName = "&" + varName;
                    varNames += ((varNames == "") ? "" : ",") + varName;
                    varName = "";
                }
            }
            string_const = setperserData[i].substr(0, 6) + '"' + formatSpecifiers.substr(0, formatSpecifiers.length() - 1) + '"' + "," + varNames + ')' + ';';

            setperserData[i] = string_const;
            setperserData.erase((setperserData.begin() + i + 1));
        }
    }
}
std::string arrrangeDebugger(std::string getData)
{
    std::string newgetData = "";
    for (int i = 0; i < getData.length(); i++)
    {
        if (getData[i] == '=' || getData[i] == ',')
        {
            newgetData += ' ';
            newgetData += getData[i];
            newgetData += ' ';
        }
        else
        {
            newgetData += getData[i];
        }
    }
    return newgetData;
}
void IOparser(std::string getData)
{
    getData = arrrangeDebugger(getData);
    //std::cout << getData << "\n";
    std::vector<std::string> tokenContainer;
    std::string ins_string = "";
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
    /*for (int i = 0; i < tokenContainer.size(); i++)
    {
        std::cout << tokenContainer[i] << "\n";
    }*/
    for (int i = 1; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i] == "=" || tokenContainer[i] == ",")
        {
            varibaleMapper.insert({tokenContainer[i - 1], datamapper.find((datamapper.find(tokenContainer[0])->second))->second});
        }
    }
    // for(auto it:varibaleMapper){
    //    std::cout<<it.first<<" "<<it.second<<"\n";
    //}
}
void conditionBuilder(std::string parse)
{
    int cnt = 0, cnt2 = 0;
    for (int i = 0; i < parse.length(); i++)
    {
        if (parse[i] == '?')
        {
            cnt++;
        }
        if (parse[i] == 'i' || parse[i] == 'e')
        {
            break;
        }
        cnt2++;
    }
    if (parse[parse.length() - 1] != '>')
    {
        parse = parse.substr(cnt2, parse.length() - cnt2);
        if (parse.substr(0, 2) == "if")
        {
            parse += "{";
            setperserData.push_back(parse);
        }
        else if (parse.substr(0, 4) == "elif")
        {
            std::string sf = "else if";
            parse = sf + parse.substr(4, parse.length() - 4) + "{";
            setperserData.push_back(parse);
        }
        else if (parse.substr(0, 4) == "else")
        {
            parse += "{";
            setperserData.push_back(parse);
        }
    }
    else
    {
        std::string stf = "}";
        setperserData.push_back(stf);
    }
}
void iteratorBuilders(std::string parse)
{
    if (parse[parse.length() - 1] != '>')
    {
        parse = parse.substr(2, parse.length() - 2);
        parse = spaceDebug(parse);
        if (parse[0] == 'f')
        {
            for (int i = 0; i < parse.length(); i++)
            {
                if (parse[i] == ',')
                {
                    parse[i] = ';';
                }
            }
            parse = "for" + parse.substr(1, parse.length() - 1) + "{";
            setperserData.push_back(parse);
        }
        else if (parse[0] == 'w')
        {
            parse = "while" + parse.substr(1, parse.length() - 1) + "{";
            setperserData.push_back(parse);
        }
    }
    else
    {
        setperserData.push_back("}");
    }
}

void Parser(std::string getData)
{
    if (getData[0] == '<')
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
        else if (getData[getData.length() - 1] == '>' && getData[getData.length() - 2] == '/')
        {
            if (getData.find('f') != std::string::npos && getData[getData.find('f') + 1] == 'x')
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                std::string ins_string = getData.substr(3, getData.length() - 3) + ";";
                setperserData.push_back(ins_string);
            }
            else if (getData.find('t') != std::string::npos && getData.substr(getData.find('t'), 5) == "throw")
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                std::string ins_string = "return " + getData.substr(6, getData.length() - 6) + ";";
                setperserData.push_back(ins_string);
            }
            else if (getData[getData.length() - 3] == '/')
            {
                getData = getData.substr(1, getData.length() - 4);
                getData = spaceDebug(getData) + " ";
                std::vector<std::string> tmp;
                std::string stf;
                for (int i = 0; i < getData.length(); i++)
                {
                    if (getData[i] != ' ' && getData[i] != ',')
                    {
                        stf += getData[i];
                    }
                    else
                    {
                        if (stf.length() != 0)
                            tmp.push_back(stf);
                        stf = "";
                    }
                }
                if (tmp[0] == "take")
                {
                    std::string tmpStr = "";
                    setperserData.push_back(datamapper.find(tmp[0])->second);
                    for (int it = 1; it < tmp.size(); it++)
                    {
                        tmpStr += tmp[it] + " ";
                    }
                    setperserData.push_back(tmpStr);
                }
            }
            else
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                setperserData.push_back(datamapper.find(getData.substr(0, 2))->second + getData.substr(2, getData.length() - 2) + ";");
                IOparser(getData + ',');
            }
        }
        else if (getData[1] != '/')
        {
            if (getData.find('f') != std::string::npos && getData[getData.find('f') + 1] == 'x')
            {
                getData = getData.substr(1, getData.length() - 2);
                getData = spaceDebug(getData);
                std::string str_builder = "";
                for (int i = 0; i < getData.length(); i++)
                {
                    if (getData[i] == '(' || getData[i] == ')' || getData[i] == ',')
                    {
                        str_builder += ' ';
                        str_builder += getData[i];
                        str_builder += ' ';
                    }
                    else
                    {
                        str_builder += getData[i];
                    }
                }
                getData = str_builder;
                str_builder = "";
                std::string stf = "";
                for (int i = 0; i < getData.length(); i++)
                {
                    if (getData[i] != ' ')
                    {
                        str_builder += getData[i];
                    }
                    else
                    {
                        if (str_builder == "in" || str_builder == "ch" || str_builder == "fl")
                        {
                            stf += datamapper.find(str_builder)->second + ' ';
                        }
                        else if (str_builder == "void")
                        {
                            stf += str_builder + ' ';
                        }
                        else
                        {
                            stf += str_builder;
                        }
                        str_builder = "";
                    }
                }
                functionHeader.push_back(stf.substr(2, stf.length() - 2) + ";");
                stf += "{";
                setperserData.push_back(stf.substr(2, stf.length() - 2));
            }
            else if (getData[1] == '%')
            {
                std::string getExpression = getData.substr(2, getData.length() - 4);
                getExpression = spaceDebug(getExpression) + ";";
                setperserData.push_back(getExpression);
            }
            else
            {
                std::string getTag = stripBrace(getData);
                if (datamapper.find(getTag) != datamapper.end())
                {
                    setperserData.push_back(datamapper.find(getTag)->second);
                }
            }
        }
        else if (getData[1] == '/')
        {
            if (getData.substr(2, 3) == "log")
                setperserData.push_back(datamapper.find("/log")->second);
            else
                setperserData.push_back(datamapper.find("/")->second);
        }
    }
    else
    {
        if (setperserData[setperserData.size() - 1] == "printf()")
        {
            setperserData.push_back(getData);
        }
        else
        {
            setperserData[setperserData.size() - 1] += (" " + getData);
        }
    }
}

//int main()
int main(int argc, const char *argv[])
{
    refDataset();
    std::string getSyntax;
    std::ifstream readData(argv[1]);
    while (getline(readData, getSyntax))
    {
        getSyntax = spaceDebug(getSyntax);
        if (getSyntax.length() != 0)
        {
            if (getSyntax.substr(0, 2) == "<?" || getSyntax.substr(getSyntax.length() - 2, 2) == "?>")
            {
                conditionBuilder(getSyntax);
            }
            else if (getSyntax.substr(0, 2) == "<#" || getSyntax.substr(getSyntax.length() - 2, 2) == "#>")
            {
                iteratorBuilders(getSyntax);
            }
            else
            {
                Parser(getSyntax);
            }
        }
        else
        {
            continue;
        }
    }
    printParser();
    /*for (auto it : setperserData)
    {
        std::cout << it << "\n";
    }*/
    writeCode();
    //return 0;
}