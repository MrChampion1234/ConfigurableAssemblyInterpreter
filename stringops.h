// -*- grammar-ext: .cpp -*-
#include <string>
#include <vector>

using stringPair_t = std::pair<std::string,std::string>; 

std::string strip(std::string str, char c);
std::string stripends(std::string str, char c);

stringPair_t splitOnFirstChar(std::string str, char c);
std::vector<int> processArrayString(std::string str);