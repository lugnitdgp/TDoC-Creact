#/
import sys

dataStream = []
Parsed_Data = []
Dataset = {
    "htpl": "#include<stdio.h>\n\n",
    "main": "void main(){\n\n",
    "log": "printf()",
    "in": "int",
    "ch": "char",
    "int": "%d",
    "char": "%c",
    "take": "scanf()",
    "elif": "else if"
}

def readCode():
    global dataStream
    file = sys.argv[1]
    f = open(file, "r")
    while True:
        line = f.readline()
        if not line:
            break
        if line.strip() == '':
            continue
        dataStream.append(line.strip())       
    f.close()


def writeCode(data):
    f= open("output.c", "w")
    for i in data:
        f.write(i)
    f.close()

def tag_check(line):
    i = 1
    if line[1]=="/":
        i=2
    tag_len = line.find(">")
    if tag_len ==-1:
        return None, None

    else:
        tag = line[i:tag_len].strip()
        if tag in Dataset:
            return tag, tag_len+1
        else: return None, None

def Parser(data):
    for i in range(len(data)):
        line = data[i]
        if line[0] == "<":
            tag, tag_len = tag_check(line)
            if tag:
                if line[1] != "/":
                    Parsed_Data.append(Dataset[tag])
                    if tag_len!=len(line):   
                        newLine = [line[tag_len:]] 
                        Parser(newLine)
                else:
                    if tag=="main":
                        Parsed_Data.append("}")
                    elif tag =="htpl":
                        continue
                    
                    else:
                        Parsed_Data.append(line[:tag_len])
                        if tag_len!=len(line):   
                            newLine = [line[tag_len:]]
                            print(newLine)
                            Parser(newLine)
                
        else:
            if('<') in line:
                distance = line.find('<')
                text = line[:distance].strip()
                Parsed_Data.append(text)
                newLine = [line.replace(text, '').strip()]
                Parser(newLine)
            else:
                Parsed_Data.append(line.strip())

def printf_parser(Data):
    for i in range(len(Data)):
        String = Data[i]
        if "printf(" in String:
            Data[i] = String[:-1]
            Data[i+1] = '"'+Data[i+1]
        
        if "</log>" in String:
            Data[i-1] = Data[i-1]+"\\n"+ '"' + ')'
            Data[i]= ';\n\n'

def main():
    readCode()
    Parser(dataStream)
    if "printf()" in Parsed_Data:
        printf_parser(Parsed_Data)

    writeCode(Parsed_Data)


if __name__ == "__main__":
    main()