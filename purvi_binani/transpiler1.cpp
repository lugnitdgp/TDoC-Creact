#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <time.h>

std::unordered_map<std::string, std::string> dataMapper;
std::unordered_map<std::string, std::string> variableMapper;
std::vector<std::string> setParserData;
std::vector<std::string> functionHeader;
std::vector<std::string> headers;
std::vector<std::string> var_keeper;
std::unordered_map<std::string, std::string> vectorCounter;

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
            for (int k = 0; k < headers.size(); k++)
            {
                writeFile << headers[k] << "\n";
            }
            for (int k = 0; k < functionHeader.size(); k++)
            {
                writeFile << functionHeader[k] << "\n";
            }
            for (int k = 0; k < var_keeper.size(); k++)
            {
                writeFile << var_keeper[k] << "\n";
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

void memoryPlay(std::string getData)
{
    getData = getData.substr(2, getData.length() - 4);
    getData = spaceDebug(getData);
    if (getData.substr(0, 6) == "stream")
    {
        headers.push_back("#include <stdlib.h>");
        headers.push_back("#define predefsz 2");
        std::string _datatype = getData.substr(getData.find('(') + 1, getData.find(')') - getData.find('(') - 1);

        if (_datatype == "in")
        {
            srand(time(0));
            std::string varRect = getData.substr(getData.find(')') + 2, getData.length() - getData.find(')') + 1);
            std::string sizeDef = varRect + std::to_string(rand()).substr(0, 3);
            std::string ins_var = "c" + std::to_string(rand()).substr(0, 3);

            vectorCounter.insert({varRect, ins_var});
            vectorCounter.insert({ins_var, sizeDef});
            functionHeader.push_back("int " + ins_var + "= 0;");
            setParserData.push_back("int " + sizeDef + " = " + "predefsz;");
            std::string instance1 = dataMapper.find(_datatype)->second + " " + '*' + varRect + " = (int*)malloc(sizeof(int)*" + sizeDef + ");";
            setParserData.push_back(instance1);
            if (var_keeper.empty())
            {
                std::string ins_var1;
                std::ifstream readFile("./modules/memoryalloc.txt");
                while (getline(readFile, ins_var1))
                {
                    var_keeper.push_back(ins_var1);
                }
            }
        }
    }
    else
    {
        if (getData.substr(getData.find('.') + 1, 4) == "plus")
        {
            std::string ins_var = getData.substr(0, getData.find('.')) + "=" + "checkout(" + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ",&" + vectorCounter.find(vectorCounter.find(getData.substr(0, getData.find('.')))->second)->second + ',' + getData.substr(0, getData.find('.')) + ");";
            std::string ins_var3 = "*(" + getData.substr(0, getData.find('.')) + "+" + vectorCounter.find(getData.substr(0, getData.find('.')))->second + "++)=" + getData.substr(getData.find('(') + 1, getData.length() - getData.find('(') - 2) + ";";
            setParserData.push_back(ins_var + ins_var3);
        }
        else if (getData.substr(getData.find('.') + 1, 5) == "minus")
        {
            setParserData.push_back(vectorCounter.find(getData.substr(0, getData.find('.')))->second + "--" + ';');
        }
        else if (getData.substr(getData.find('.') + 1, 4) == "show")
        {
            setParserData.push_back("show(" + getData.substr(0, getData.find('.')) + "," + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ");");
        }
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
                if (getData.find('[') == std::string::npos)
                {
                    std::string getExpression = getData.substr(2, getData.length() - 4);
                    getExpression = spaceDebug(getExpression) + ";";
                    setParserData.push_back(getExpression);
                }
                else
                {
                    int c1 = 0, c2 = 0;
                    getData = getData.substr(2, getData.length() - 4);
                    getData = spaceDebug(getData);
                    std::string p1 = spaceDebug(getData.substr(0, getData.find('=')));
                    std::string p2 = spaceDebug(getData.substr(getData.find('=') + 1, getData.length() - getData.find('=') - 1));
                    if (p1.find('[') != std::string::npos && p1.find(']') != std::string::npos)
                    {
                        std::string var_name = p1.substr(0, p1.find('['));
                        std::string val = p1.substr(p1.find('[') + 1, p1.find(']') - p1.find('[') - 1);
                        p1 = "*(" + var_name + "+" + val + ")";
                    }
                    if (p2.find('[') != std::string::npos && p2.find(']') != std::string::npos)
                    {
                        std::string var_name2 = p2.substr(0, p2.find('['));
                        std::string val2 = p2.substr(p2.find('[') + 1, p2.find(']') - p2.find('[') - 1);
                        p2 = "*(" + var_name2 + "+" + val2 + ")";
                    }
                    setParserData.push_back(p1 + " = " + p2 + ";");
                }
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
        else if (getSyntax.substr(0, 2) == "<<")
        {
            memoryPlay(getSyntax);
        }
        else
        {
            Parser(getSyntax);
        }
    }
    printParser();
    writeCode();
}