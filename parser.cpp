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

void setVariableNames(std::string statement, const std::string &dataType) {
  int position = 0;
  while (position < (int)statement.size()) {
    std::size_t stPos = statement.find_first_not_of(" =,", position);
    std::size_t enPos = statement.find_first_of(" =,\n", stPos);
    if (stPos == std::string::npos) {
      break;
    }
    if (enPos == std::string::npos) {
      enPos = statement.size();
    }
    std::string currentVariableName = statement.substr(stPos, enPos - stPos);
    variablesDataTypeMap[currentVariableName] = tagsMap[dataType];
    position = enPos;
  }
}

void handleInputFromUser(std::string &token) {
  // erase last two chars
  token.erase(token.size() - 2, 2);
  std::stringstream ss(token);
  std::string varName;
  std::string finalScanfParam = "scanf(\"";
  std::string variableNames = "";
  ss >> varName;  // to ignore the take at front
  while ((ss >> varName)) {
    finalScanfParam += typeSpecifierMap[variablesDataTypeMap[varName]];
    variableNames += "&" + varName + ", ";
  }
  finalScanfParam +=
      "\"," + variableNames.substr(0, variableNames.size() - 2) + ");";
  token = finalScanfParam;
}

void processTokens(std::vector<std::string> &tokens) {
  for (int i = 0; i < (int)tokens.size(); i++) {
    if (tokens[i].size() > 2 && tokens[i][tokens[i].size() - 1] == '/' &&
        tokens[i][tokens[i].size() - 2] == '/') {
      // taking input from user
      handleInputFromUser(tokens[i]);
    } else if (*tokens[i].rbegin() == '/') {
      // some variable declaration occurring
      std::string dataType = tokens[i].substr(0, 2);
      std::string variableDeclarations =
          tokens[i].substr(2, tokens[i].size() - 3);
      setVariableNames(variableDeclarations, dataType);
      tokens[i] = tagsMap[dataType] + " " + variableDeclarations + ";";
    } else if (tokens[i] == "printf()" && tokens[i + 2] == ";") {
      std::string &currentToken = tokens[i + 1];
      std::string formatString = "", variableNamesWithComma = "";

      for (int pos = 0; pos < (int)currentToken.size(); pos++) {
        // check is pos and pos+1 is ${
        if (pos < (int)currentToken.size() - 1 && currentToken[pos] == '$' &&
            currentToken[pos + 1] == '{') {
          // get the variable name

          bool varMatchFound = false;

          std::size_t varEndPos = currentToken.find_first_of('}', pos + 2);
          if (varEndPos != std::string::npos) {
            std::string varName =
                currentToken.substr(pos + 2, varEndPos - pos - 2);

            if (variablesDataTypeMap.find(varName) !=
                variablesDataTypeMap.end()) {
              formatString += typeSpecifierMap[variablesDataTypeMap[varName]];
              variableNamesWithComma += varName + ", ";
              varMatchFound = true;
            }
          }

          if (!varMatchFound) {
            formatString += "$";
          } else {
            pos = varEndPos;
          }
        } else {
          formatString += currentToken[pos];
        }
      }
      variableNamesWithComma =
          variableNamesWithComma.substr(0, variableNamesWithComma.size() - 2);
      tokens[i] =
          "printf(\"" + formatString + "\"" +
          (variableNamesWithComma.size() ? ", " + variableNamesWithComma : "") +
          ");";
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
