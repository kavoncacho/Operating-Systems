/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#include "dicttree.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

//helper method
std::vector<std::string> splitStringByDelimiters(std::string string, std::string delimiters) {
    int index = 0;
    int lastindex = 0;
    std::vector<std::string> found_strings;
    for (char character : string) { // loop through each character in string being split
        if (delimiters.find(character) != std::string::npos) { // check if current character is contained in delimiter string
            found_strings.push_back(string.substr(lastindex, index - lastindex)); // push back word once delimiter is reached
            lastindex = index + 1;
        }
        index++;
    }
    return found_strings;
}

int main(int argc, char** argv) {
    
    //parameter error check
    if (argc != 3) {
        printf("\n Please use the proper amount of parameters (2).\n\n");
        return -1;
    }

    //file existence check
    for (int i = 1; i <= 2; i++) {
        std::ifstream fileCheck(argv[i]);
        if (!fileCheck.is_open()) {
            printf("\n Please make sure parameter %d exists. \n\n", i);
            return -1;
        }
        fileCheck.close();
    }
    
    // construct dictionary
    DictNode root = DictNode(false);

    std::ifstream dictStream(argv[1]); // stream to first parameter
    std::string line;
    while (std::getline(dictStream, line)) {
        root.add(line.c_str());
    }

    std::ifstream wordStream(argv[2]); // stream to second parameter
    while (std::getline(wordStream, line, '\0')) { // getline default delimiter is overwritten with \0 to prevent conflict with helper method
        std::vector<std::string> words = splitStringByDelimiters(line, "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~");
        for (std::string word : words) {
            int count = 0;
            DictNode* endNode = root.findEndingNodeOfAStr(word.c_str(), word.c_str());
            if (endNode == nullptr) {
                printf("%s 0\n", word.c_str());
            } else {
                if (strlen(word.c_str()) == 0) {
                    continue;
                }
                endNode->countWordsStartingFromANode(count);
                printf("%s %d\n", word.c_str(), count);
            }
        }
    }

    return 0;
}