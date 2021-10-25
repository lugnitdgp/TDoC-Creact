#include<iostream>
#include<fstream>
#include<unordered_map>
#include<vector>
#include<string>
#include<time.h>

//symo=bol table for mapping the creact commands to C commands
std::unordered_map<std::string, std::string> dataMapper;

//map for keeping the variable types
std::unordered_map<std::string, std::string>variableMapper;

//vector to store the individual statements converted to C
std::vector<std::string> setParserData;

std::vector<std::string> functionHeader;

//vector to store the header fiels for vector implementation
std::vector<std::string> headers;

//vector to store strings of functions passed as dynamic array
std::vector<std::string> var_keeper;

//vector counter map to keep the vectors count
std::unordered_map<std::string, std::string> vectorCounter;

//dataMapper reference dataset -- stores the necessary mappings
void refDataset(){
    dataMapper.insert({"htpl" , "#include<stdio.h>\n"});
    dataMapper.insert({"main" , "void main(){\n"});
    dataMapper.insert({"log", "printf()"});
    dataMapper.insert({"/","}"});
    dataMapper.insert({"/log", ";"});
    dataMapper.insert({"in", "int"});
    dataMapper.insert({"ch", "char"});
    dataMapper.insert({"fl", "float"});
    dataMapper.insert({"int", "%d"});
    dataMapper.insert({"char", "%c"});
    dataMapper.insert({"float", "%f"});
    dataMapper.insert({"take","scanf()"});

}

//write code to c file 
void writeCode(){
    std::ofstream writeFile("output.c");
    for(int i=0; i<setParserData.size()-1; i++){
        // std::cout<<setParserData[i]<<" ";
        writeFile<<setParserData[i]<<"\n";
        if(i==0){
            for(int k=0; k<headers.size(); k++){
                writeFile << headers[k] <<"\n"; 
            }
            for(int k=0; k<functionHeader.size(); k++){
                writeFile << functionHeader[k] <<"\n"; 
            }
            for(int k=0; k<var_keeper.size(); k++){
                writeFile << var_keeper[k] <<"\n"; 
            }
        }
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

//tokenizing expression inside of printf()
std::vector<std::string> fxEval(std::string st){
    std::vector<std::string> store;
    std::string evalgetData;
    for(int i=0; i<st.length(); i++){
        if(st[i] == '+' || st[i] == '-' || st[i] == '*' || st[i] == '/' || st[i] == '%' || st[i] == '^'){
            evalgetData += ' ';
            evalgetData += st[i];
            evalgetData += ' ';
        }
        else{
            evalgetData += st[i];
        }
    }
    evalgetData += ' ';
    std::string helper_st = "";
    for(int i=0; i<evalgetData.length(); i++){
        if(evalgetData[i] != ' '){
            helper_st += evalgetData[i];
        }
        else{
            store.push_back(helper_st);
            helper_st = "";
        }
    } 
    return store;
}

//function for constructing printf in c
void printParser(){
    std::string string_const;
    for(int i=0;i<setParserData.size(); i++){

        if(setParserData[i]=="printf()"){
            // hello ${var3} \n
            int pos;
            std::string formatSpecifiers = "", varNames="", var="";
            std::vector<std::string> expressEval;
            for(int j=0; j<setParserData[i + 1].length(); j++){

                if(setParserData[i + 1].substr(j, 2) == "${"){

                    pos = setParserData[i + 1].find("}", j);
                    var = setParserData[i + 1].substr(j+2, pos-(j+2));
                    expressEval = fxEval(var);
                    int check = 1;
                    for(int k=0; k<expressEval.size(); i++){
                        if(variableMapper.find(expressEval[k])!=variableMapper.end()){

                            formatSpecifiers += variableMapper.find(expressEval[k])->second;
                            varNames += ((varNames == "") ? "":",") + var;
                            check = 0;
                            break;
                        }
                    }
                    
                    if(check==1){
                        //parses anything to string
                        formatSpecifiers += setParserData[i+1].substr(j, pos-j+1);
                    }

                    j = pos;
                }
                else{

                    formatSpecifiers += setParserData[i+1][j];
                }
            }

            string_const = setParserData[i].substr(0,7) + '"' + formatSpecifiers + '"' + ((varNames == "") ? "" : ",") +  ((varNames == "") ? "" : "(")  +varNames+ ((varNames == "") ? "" : ")")  + ")" + setParserData[i+2];
            setParserData[i] = string_const;
            setParserData.erase(std::next(setParserData.begin(), i+1), std::next(setParserData.begin(), i+3));
        }
        else if(setParserData[i]=="scanf()"){
            std::string formatSpecifiers = "", varNames="", var="";
            for(int j=0; j<setParserData[i+1].size(); j++){
                if(setParserData[i+1][j] != ' '){
                    var  +=  setParserData[i+1][j];
                }
                else{
                    formatSpecifiers += variableMapper.find(var)->second + " ";
                    var = "&" + var;
                    varNames +=  ((varNames == "")? "" : ",") + var;
                    var="";
                }

            }
            string_const = setParserData[i].substr(0, 6) + '"' + formatSpecifiers.substr(0, formatSpecifiers.length()-1) + '"' + "," + varNames + ");";
            setParserData[i] = string_const;
            setParserData.erase((setParserData.begin()+i+1));
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


void Parser(std::string getData){
    getData = spaceDebug(getData);
    // std::cout<<getData<<" ";
    if(getData[0]=='<'){
        //for the variable declaration part
        if(getData[getData.length()-2]=='/' && getData[getData.length()-1]=='>'){
           
        // <fx var= func(params) />  
        //func
           if((getData.find('f') != std::string::npos) && getData[getData.find('f')+1]=='x'){
               getData = getData.substr(1, getData.length()-3);
               getData = spaceDebug(getData);
               std::string instance_string = getData.substr(3, getData.length()-3)+";";
               setParserData.push_back(instance_string);
           }
           //throw part
           else if(getData.find('t') != std::string::npos && getData.substr(getData.find('t'), 5) == "throw"){
               getData = getData.substr(1, getData.length()-3);
               getData = spaceDebug(getData);
               std::string instance_string = "return " + getData.substr(6, getData.length()-6)+";";
               setParserData.push_back(instance_string);
           }
           else  if(getData[getData.length()-3]=='/'){
                getData = getData.substr(1, getData.length()-4);
                getData = spaceDebug(getData)+" ";
                std::vector<std::string>temp;
                std::string st;
                for(int i=0; i<getData.length(); i++){
                    if(getData[i] != ' ' && getData[i] != ','){
                        st = st + getData[i];
                    }
                    else{
                        if(st.length()!=0)
                            temp.push_back(st);
                            st = "";
                    }
                }
                if(temp[0]=="take"){
                    std::string temp_string = "";
                    setParserData.push_back((dataMapper.find(temp[0])->second));
                    for(int it=1; it<temp.size(); it++){
                        temp_string += temp[it] + " ";
                    }
                    setParserData.push_back(temp_string);
                }
            }
            else{
                getData = getData.substr(1, getData.length()-3);
                getData = spaceDebug(getData);
                //inserting the entire delcaration fo the variable
                setParserData.push_back(dataMapper.find(getData.substr(0,2))->second + getData.substr(2, getData.length()-2) + ";" );
                IOparser(getData+',');
            }
            
        }   
        else if(getData[1] !='/'){
            if(getData.find('f')!=std::string::npos && getData[getData.find('f')+1]=='x'){
            
            getData = getData.substr(1, getData.length()-2);
            getData = spaceDebug(getData);
            // std::cout<<getData<<" ";
            std::string string_builder = "";
            for(int i=0; i<getData.length(); i++){
                if(getData[i] == '(' || getData[i] == ')' || getData[i] == ','){
                    //don't use +=  for string here
                    string_builder = string_builder + ' ' + getData[i] + ' '; 
                }
                else{
                    string_builder += getData[i];
                }
            }
            // std::cout<<string_builder<<" ";
            getData  = string_builder;
            string_builder = "";
            std::string stf = "";
            for(int i=0; i<getData[i]; i++){
                if(getData[i]!=' '){
                    string_builder += getData[i];
                } else{
                    if(string_builder=="in" || string_builder=="ch" || string_builder=="fl"){
                        stf += dataMapper.find(string_builder)->second + ' ';
                    } else if(string_builder=="void"){
                        stf += string_builder + ' '; 
                    } else{
                        stf += string_builder;
                    }
                    string_builder= "";
                }
            }
            functionHeader.push_back(stf.substr(2, stf.length()-2)+";");
            stf += "{";
            setParserData.push_back(stf.substr(2, stf.length()-2));

        }
        //arithmetic operations
        else if(getData[1] == '%'){
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

        else {
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
        }

        } 
        else if(getData[1]=='/'){
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

void iteratorBuilder(std::string parse){
    if(parse[parse.length()-1] != '>'){
        parse = parse.substr(2, parse.length()-2);
        parse = spaceDebug(parse);
        if(parse[0]=='f'){
            for(int i=0; i<parse.length(); i++){
                if(parse[i]==',')
                    parse[i] = ';';
            }
            parse = "for" + parse.substr(1, parse.length()-1) + "{";
            setParserData.push_back(parse);
        } else if(parse[0]=='w'){
            parse = "while" + parse.substr(1, parse.length()-1) + "{";
            setParserData.push_back(parse);
        }

    } else{
        setParserData.push_back("}");
    }
}

void memoryManip(std::string getData)
{
    getData = getData.substr(2, getData.length() - 4);
    getData = spaceDebug(getData);
    std::string _datatype = getData.substr(getData.find('(') + 1, getData.find(')') - getData.find('(') - 1);
    // std::cout<<_datatype<<" ";
    if (getData.substr(0, 6) == "stream")
    {
        headers.push_back("#include <stdlib.h>");
        headers.push_back("#define predefsz 2");
        headers.push_back("#define predefsz1 4");

        if (_datatype == "in" || _datatype=="fl")
        {
            
            std::string varRect = getData.substr(getData.find(')') + 2, getData.length() - getData.find(')') + 1);
            std::string sizeDef = varRect + std::to_string(rand()).substr(0, 3);
            std::string ins_var = "c" + std::to_string(rand()).substr(0, 3);

            vectorCounter.insert({varRect, ins_var});
            vectorCounter.insert({ins_var, sizeDef});
           
            if(_datatype == "in"){
                functionHeader.push_back("int " + ins_var + "= 0;");
                setParserData.push_back("int " + sizeDef + " = " + "predefsz;");
                std::string instance1 = dataMapper.find(_datatype)->second + " " + '*' + varRect + " = (int*)malloc(sizeof(int)*" + sizeDef + ");";
                setParserData.push_back(instance1);
            } 
            else if(_datatype == "fl"){
                functionHeader.push_back("int " + ins_var + "= 0;");
                setParserData.push_back("float " + sizeDef + " = " + "predefsz1;");
                std::string instance1 = dataMapper.find(_datatype)->second + " " + '*' + varRect + " = (float*)malloc(sizeof(float)*" + sizeDef + ");";
                setParserData.push_back(instance1);
            }
            // std::cout<<_datatype<<" ";
            
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
        std::string _datatype1 = getData.substr(getData.find('(') + 1, getData.find(')') - getData.find('(') - 1);
        // std::cout<<getData;
        if (getData.substr(getData.find('.') + 1, 4) == "plus")
        {   
            std::string ins_var;
            ins_var = getData.substr(0, getData.find('.')) + "=" + "checkout(" + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ",&" + vectorCounter.find(vectorCounter.find(getData.substr(0, getData.find('.')))->second)->second + ',' + getData.substr(0, getData.find('.')) + ");";
            // ins_var = getData.substr(0, getData.find('.')) + "=" + "checkout1(" + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ",&" + vectorCounter.find(vectorCounter.find(getData.substr(0, getData.find('.')))->second)->second + ',' + getData.substr(0, getData.find('.')) + ");";  
            std::string ins_var3 = "*(" + getData.substr(0, getData.find('.')) + "+" + vectorCounter.find(getData.substr(0, getData.find('.')))->second + "++)=" + getData.substr(getData.find('(') + 1, getData.length() - getData.find('(') - 2) + ";";
            setParserData.push_back(ins_var + ins_var3);
        }
        else if (getData.substr(getData.find('.') + 1, 5) == "minus")
        {
            setParserData.push_back(vectorCounter.find(getData.substr(0, getData.find('.')))->second + "--" + ';');
        }
        else if (getData.substr(getData.find('.') + 1, 4) == "show")
        {
            // if(_datatype=="in")
            setParserData.push_back("show(" + getData.substr(0, getData.find('.')) + "," + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ");");
            // else if(_datatype=="fl")  
                // setParserData.push_back("show1(" + getData.substr(0, getData.find('.')) + "," + vectorCounter.find(getData.substr(0, getData.find('.')))->second + ");");
        }
    }
}

int main(int argc, char const *argv[])
{   
    srand(time(0));
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
        else if (getSyntax.substr(0, 2) == "<#" || getSyntax.substr(getSyntax.length() - 2, 2) == "#>"){
            iteratorBuilder(getSyntax);
        }
        else if(getSyntax.substr(0, 2)=="<<"){
            memoryManip(getSyntax);
        }
        else if (getSyntax.substr(0, 4) == "<!--" || getSyntax.substr(getSyntax.length() - 3, 3) == "-->"){
            continue;
        }
        else
        {
            Parser(getSyntax);
        }
    }
    printParser();
    writeCode();
}