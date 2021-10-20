#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "table.h"

void parseFile(const std::string &file_name);
std::string fileReader(const std::string &file_name);

#endif  // PARSER_H
