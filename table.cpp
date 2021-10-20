#include "./table.h"

std::unordered_map<std::string, std::string> tagsMap = {
    {"htpl", "#include<stdio.h>"},
    {"main", "int main() {"},
    {"log", "printf()"},
    {"take", "scanf()"},
    {"/", "}"},
    {"/log", ";"},
    {"in", "int"},
    {"fl", "float"},
    {"ch", "char"},
};

std::unordered_map<std::string, std::string> variablesDataTypeMap = {};

std::unordered_map<std::string, std::string> typeSpecifierMap = {
    {"int", "%d"},
    {"char", "%c"},
    {"float", "%f"},
};
