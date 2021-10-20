#include "./table.h"

std::unordered_map<std::string, std::string> tagsMap = {
    {"htpl", "#include<stdio.h>"},
    {"main", "int main() {"},
    {"log", "printf()"},
    {"/", "}"},
    {"/log", ";"}};
