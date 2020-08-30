#include <string>
#include <vector>
#include <algorithm>
#ifndef STRINGOPS_CPP
#define STRINGOPS_CPP

// Some helper functions for processing text
// For the splitOnFirstChar function
using stringPair_t = std::pair<std::string,std::string>; 
// Strip the given string of a given character
std::string strip(std::string str, char c) {
	//printf("Entering strip function with args '%s' '%c'\n", str.c_str(), c);
	std::string newString{ "", 0 };
	
	// Since newString won't be longer than str,
	// reserve a space that's the same as str
	//bool temp;
	newString.reserve(str.length());
	for (int i = 0; i < (int)str.length(); ++i) {
		//temp = (str[i] != c);
		//printf("%s '%c'\n", temp ? "true" : "false", c);
		if (str[i] != c) {
			//printf("Pushing '%c' to newString\n", c);
			newString.push_back(str[i]);
		}
	}
	//printf("Exiting strip function with output '%s'\n", newString.c_str());
	return newString;
}

std::string stripends(std::string str, char c) {
	if (str.empty()) {
		return str;
	}
	std::size_t ind;
	if (str.back() == c) {
		ind = str.find_last_not_of(c);
		//printf("end ind: %i\n", (int)ind);
		str.erase(ind+1);
	}
	if (str.front() == c) {
		ind = str.find_first_not_of(c);
		//printf("start ind: %i\n", (int)ind);
		str.erase(0, ind);
	}
	
	return str;
}

// Returns a pair of the string split by the first instance of char in str 
// For some weird reason, this is vaguely lisp-like in what it does.
// Probably because it's like it's using a cons object
stringPair_t splitOnFirstChar(std::string str, char c) {
	int i = str.find_first_of(c);
	//printf("splitting at %i\n", i);
	if (i == -1) {
		// Cover edge case where char is not found 
		// Returns a pair of the input string and an empty string
		return std::make_pair(str, std::string{""});
	}
	return std::make_pair(str.substr(0,i), str.substr(i+1));
}

// Takes a variable str which looks like an array, and returns a vector 
// representing that array
std::vector<int> processArrayString(std::string str) {
	std::vector<int> arr;
	int approxLen = str.length();
	approxLen = approxLen / 2;
	arr.reserve(approxLen);
	str = strip(str, ' '); // Strip spaces from entire string
	str = strip(str, '['); str = strip(str, ']'); // Strip brackets
	stringPair_t tempPair;
	
	// Count how many commas there are
	int numCommas = std::count(str.begin(), str.end(), ',');
	
	// Continuously split array in two parts by a comma
	// numCommas+1 is there to make sure that if 0 commas are found,
	// then the for loop will still run at least once
	// Then, it makes sure that for each new comma, the for loop is run once more
	for (int i=0; i < (numCommas+1); i++) {
		// Just before exiting loop. Means that the current string 
		// is just a number and can be interpreted directly
		if (i == numCommas) { 
			arr.push_back(stoi(str));
		} else {
			// Split string in two parts 
			tempPair = splitOnFirstChar(str, ',');
			
			// Push the first value to the vector...
			arr.push_back(stoi(tempPair.first));
			
			// ... and replace the current string with the second half of the string
			str = tempPair.second;
		}
	}
	return arr;
}
#endif