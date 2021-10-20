#include <iostream>

#include "parser.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: transpiler [CREACT SOURCE CODE]" << std::endl;
    return 1;
  } else {
    std::string file_name = argv[1];
    std::cout << "Transpiling " << file_name << std::endl;
    parseFile(file_name);
    return 0;
  }
  return 0;
}
