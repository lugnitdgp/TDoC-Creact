#include "parser.h"

#include "table.h"

std::string fileReader(const std::string &file_name) {
  std::ifstream file(file_name);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void fileWriter(std::vector<std::string> tokens) {
  std::ofstream file;
  file.open("output.c");
  for (auto &token : tokens) {
    file << token << "\n";
  }
  file.close();
}

void replaceNewline(std::string &text) {
  for (int i = 0; i < (int)text.size(); i++) {
    if (text[i] == '\n') {
      std::string::size_type pos = text.find_first_not_of(" ", i + 1);
      if (pos != std::string::npos) {
        text.erase(i + 1, pos - i - 1);
      }
      text[i] = ' ';
    }
  }
}

void trimStringAndPush(std::string text, std::vector<std::string> &tokens) {
  replaceNewline(text);
  std::string::size_type pos = text.find_first_not_of(" \t\n\r\f\v");
  std::string::size_type lpos = text.find_last_not_of(" \t\n\r\f\v");
  if (pos == std::string::npos) {
    // empty string
    return;
  } else {
    // replace \n from text
    tokens.push_back(text.substr(pos, lpos - pos + 1));
  }
}

void replaceTokensWithCode(std::vector<std::string> &tokens) {
  tokens.pop_back();  // to remove the last extra closing brace
  for (auto &token : tokens) {
    auto itr = tagsMap.find(token);
    if (itr != tagsMap.end()) {
      token = itr->second;
    } else if (token[0] == '/') {
      token = "}";
    }
  }
}

void processTokens(std::vector<std::string> &tokens) {
  for (int i = 0; i < (int)tokens.size(); i++) {
    if (tokens[i] == "printf()" && tokens[i + 2] == ";") {
      tokens[i] = "printf(\"" + tokens[i + 1] + "\");";
      tokens.erase(tokens.begin() + i + 1, tokens.begin() + i + 3);
    }
  }
}

void parseFile(const std::string &file_name) {
  std::string fileContent = fileReader(file_name);
  int leftPointer = 0;
  std::vector<std::string> tokens;

  while (leftPointer < (int)fileContent.size()) {
    std::size_t left = fileContent.find("<", leftPointer);
    std::size_t right = fileContent.find(">", leftPointer);

    // TODO: handle when there is no < or >
    if (left == std::string::npos || right == std::string::npos) {
      // push the rest of the string
      trimStringAndPush(fileContent.substr(leftPointer), tokens);
      break;
    }
    // when there are other characters between leftPointer and <
    if (leftPointer < (int)left) {
      std::string text = fileContent.substr(leftPointer, left - leftPointer);
      trimStringAndPush(text, tokens);
    }

    // TODO: handle when the substr is between < and > is not a correct keyword
    std::string tag = fileContent.substr(left + 1, right - left - 1);
    tokens.push_back(tag);

    leftPointer = right + 1;
  }

  replaceTokensWithCode(tokens);
  processTokens(tokens);
  fileWriter(tokens);
}
