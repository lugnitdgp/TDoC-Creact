#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, std::string> dataMapper;
std::unordered_map<std::string, std::string> variableMapper;
std::vector<std::string> setParserData;
std::vector<std::string> functionHeader;

void refDataset()
{
    dataMapper.insert({"htpl", "#include<stdio.h>\n"});
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
}

void writeCode()
{
    std::ofstream writeFile("output.c");
    for (int i = 0; i < setParserData.size() - 1; i++)
    {
        writeFile << setParserData[i] << "\n";
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
            ptr1++;
        if (getData[ptr2] == ' ')
            ptr2--;
    }
    return getData.substr(ptr1, ptr2 - ptr1 + 1);
}

std::string arrangeDebugger(std::string getData)
{
    std::string newGetData = "";
    for (int i = 0; i < getData.length(); i++)
    {
        if (getData[i] == '=' || getData[i] == ',')
        {
            newGetData += ' ';
            newGetData += getData[i];
            newGetData += ' ';
        }
        else
        {
            newGetData += getData[i];
        }
    }
    return newGetData;
}

void IOparser(std::string getData)
{
    getData = arrangeDebugger(getData);
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
    for (int i = 1; i < tokenContainer.size(); i++)
    {
        if (tokenContainer[i] == "=" || tokenContainer[i] == ",")
        {
            variableMapper.insert({tokenContainer[i - 1], dataMapper.find((dataMapper.find(tokenContainer[0])->second))->second});
        }
    }
}

std::string stripBraces(std::string getData)
{
    return getData.substr(1, getData.find('>') - 1);
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
            setParserData.push_back(parse);
        }
        else if (parse.substr(0, 4) == "elif")
        {
            std::string sf = "else if";
            parse = sf + parse.substr(4, parse.length() - 4) + "{";
            setParserData.push_back(parse);
        }
        else if (parse.substr(0, 4) == "else")
        {
            parse += "{";
            setParserData.push_back(parse);
        }
    }
    else
    {
        std::string stf = "}";
        setParserData.push_back(stf);
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
            setParserData.push_back(parse);
        }
        else if (parse[0] == 'w')
        {
            parse = "while" + parse.substr(1, parse.length() - 1) + "{";
            setParserData.push_back(parse);
        }
    }
    else
    {
        setParserData.push_back("}");
    }
}

std::vector<std::string> fxEval(std::string st)
{
    std::vector<std::string> store;
    std::string newgetData = "";
    for (int i = 0; i < st.length(); i++)
    {
        if (st[i] == '+' || st[i] == '+' || st[i] == '-' || st[i] == '*' || st[i] == '/' || st[i] == '%' || st[i] == '^')
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
    return store;
}

void printParser()
{
    std::string string_const;
    for (int i = 0; i < setParserData.size(); i++)
    {
        if (setParserData[i] == "printf()")
        {
            int pos;
            std::vector<std::string> expressEval;
            std::string formatSpecifiers = "", varNames = "", varName = "";
            for (int j = 0; j < setParserData[i + 1].size(); j++)
            {
                if (setParserData[i + 1].substr(j, 2) == "${")
                {
                    pos = setParserData[i + 1].substr(j).find('}') + j;
                    varName = setParserData[i + 1].substr(j + 2, pos - (j + 2));
                    expressEval = fxEval(varName);
                    int check = 1;
                    for (int k = 0; k < expressEval.size(); i++)
                    {
                        if (variableMapper.find(expressEval[k]) != variableMapper.end())
                        {
                            formatSpecifiers += variableMapper.find(expressEval[k])->second;
                            varNames += ((varNames == "") ? "" : ",") + varName;
                            check = 0;
                            break;
                        }
                    }

                    if (check == 1)
                    {
                        formatSpecifiers += setParserData[i + 1].substr(j, pos - j + 1);
                    }
                    j = pos;
                }
                else
                {
                    formatSpecifiers += setParserData[i + 1][j];
                }
            }
            string_const = setParserData[i].substr(0, 7) + '"' + formatSpecifiers + '"' + ((varNames == "") ? "" : ",(") + varNames + ((varNames == "") ? "" : ")") + ")" + setParserData[i + 2];
            setParserData[i] = string_const;
            setParserData.erase(std::next(setParserData.begin(), i + 1), std::next(setParserData.begin(), i + 3));
        }
        else if (setParserData[i] == "scanf()")
        {
            std::string formatSpecifiers = "", varNames = "", varName = "";
            for (int j = 0; j < setParserData[i + 1].size(); j++)
            {
                if (setParserData[i + 1][j] != ' ')
                {
                    varName += setParserData[i + 1][j];
                }
                else
                {
                    formatSpecifiers += variableMapper.find(varName)->second + " ";
                    varName = "&" + varName;
                    varNames += ((varNames == "") ? "" : ",") + varName;
                    varName = "";
                }
            }
            string_const = setParserData[i].substr(0, 6) + '"' + formatSpecifiers.substr(0, formatSpecifiers.length() - 1) + '"' + "," + varNames + ')' + ';';
            setParserData[i] = string_const;
            setParserData.erase((setParserData.begin() + i + 1));
        }
    }
}

void Parser(std::string getData)
{
    if (getData[0] == '<')
    {
        std::string getTag = stripBraces(getData);
        std::string helper_string = getData.substr(getData.find('>') + 1, getData.length() - (getData.find('>') + 1));
        if (helper_string.find('<') != std::string::npos && helper_string[helper_string.find('<') + 1] == '/')
        {

            if (dataMapper.find(getTag) != dataMapper.end())
            {
                setParserData.push_back(dataMapper.find(getTag)->second);
            }
            std::string toRemove = helper_string.substr(helper_string.find('/') - 1);
            helper_string.erase(helper_string.find(toRemove), toRemove.length());
            setParserData.push_back(helper_string);
            std::string getEndTag = stripBraces(getData.substr(getData.substr(1).find('<') + 1));
            if (getEndTag.substr(1, 3) == "log")
            {
                setParserData.push_back(dataMapper.find("/log")->second);
            }
            else
            {
                setParserData.push_back(dataMapper.find("/")->second);
            }
        }
        else if (getData[getData.length() - 1] == '>' && getData[getData.length() - 2] == '/')
        {
            if (getData.find('f') != std::string::npos && getData[getData.find('f') + 1] == 'x')
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                std::string ins_string = getData.substr(3, getData.length() - 3) + ";";
                setParserData.push_back(ins_string);
            }
            else if (getData.find('t') != std::string::npos && getData.substr(getData.find('t'), 5) == "throw")
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                std::string ins_string = "return " + getData.substr(6, getData.length() - 6) + ";";
                setParserData.push_back(ins_string);
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
                    setParserData.push_back(dataMapper.find(tmp[0])->second);
                    for (int it = 1; it < tmp.size(); it++)
                    {
                        tmpStr += tmp[it] + " ";
                    }
                    setParserData.push_back(tmpStr);
                }
            }
            else
            {
                getData = getData.substr(1, getData.length() - 3);
                getData = spaceDebug(getData);
                setParserData.push_back(dataMapper.find(getData.substr(0, 2))->second + getData.substr(2, getData.length() - 2) + ";");
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
                            stf += dataMapper.find(str_builder)->second + ' ';
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
                setParserData.push_back(stf.substr(2, stf.length() - 2));
            }
            else if (getData[1] == '%')
            {
                std::string getExpression = getData.substr(2, getData.length() - 4);
                getExpression = spaceDebug(getExpression) + ";";
                setParserData.push_back(getExpression);
            }
            else
            {
                std::string getTag = stripBraces(getData);
                if (dataMapper.find(getTag) != dataMapper.end())
                {
                    setParserData.push_back(dataMapper.find(getTag)->second);
                }
            }
        }
        else if (getData[1] == '/')
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
        {
            setParserData.push_back(getData);
        }
        else
        {
            setParserData[setParserData.size() - 1] += (" " + getData);
        }
    }
}

int main(int argc, char const *argv[])
{
    refDataset();
    std::string getSyntax;
    std::ifstream readData(argv[1]);
    while (getline(readData, getSyntax))
    {
        getSyntax = spaceDebug(getSyntax);
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
    printParser();
    writeCode();
}