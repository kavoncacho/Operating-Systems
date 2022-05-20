/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#include "countwords.h"
#include "dicttree.h"
#include "EXEC_STATUS.h"

#include <pthread.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <typeinfo>

//helper method to parse delimiters
std::vector<std::string> splitStringByDelimiters(std::string string, std::string delimiters, void * VoidPtr) {
    
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;
    
    int index = 0;
    int lastindex = 0;
    std::vector<std::string> found_strings;
    for (char character : string) { // loop through each character in string being split
        if (delimiters.find(character) != std::string::npos) { // check if current character is contained in delimiter string
            found_strings.push_back(string.substr(lastindex, index - lastindex)); // push back word once delimiter is reached
            lastindex = index + 1;
            (*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX])++;
        }
        index++;
    }
    return found_strings;
}

void * count_words_spawn(void * VoidPtr) {
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;

    while (sharedStruct->taskCompleted[DICTSRCFILEINDEX] == false) {
        //while loop to stall until first task is done
    }

    //initialize what is going to be incremented
    (*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX]) = (long) 0;
    sharedStruct->wordCountInFile[TESTFILEINDEX] = 0;

    //stream to second parameter
    std::ifstream wordStream(sharedStruct->filePath[1]);
    std::string line;
    std::ofstream outputFile;
    outputFile.open("countwords_output.txt");
    
    while (std::getline(wordStream, line, '\0')) { // getline default delimiter is overwritten with \0 to prevent conflict with helper method
        std::vector<std::string> words = splitStringByDelimiters(line, "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~", sharedStruct);
        for (std::string word : words) {
            int count = 0;
            DictNode* endNode = sharedStruct->dictRootNode->findEndingNodeOfAStr(word.c_str(), word.c_str());
            if (endNode == nullptr) { //if our count is zero
                if (sharedStruct->minNumOfWordsWithAPrefixForPrinting == 0) { //if we are able to print zeroes
                    outputFile << word.c_str() << " " << 0 << "\n";
                } else {
                    (*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX])+=word.length();
                    sharedStruct->wordCountInFile[TESTFILEINDEX]++;
                    continue;
                }
            } else {
                if (strlen(word.c_str()) == 0) {
                    continue;
                }
                endNode->countWordsStartingFromANode(count);
                if(count >= sharedStruct->minNumOfWordsWithAPrefixForPrinting) { //if the count is more than what we are able to print
                    outputFile << word.c_str() << " " << count << "\n";
                } else {
                    (*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX])+=word.length();
                    sharedStruct->wordCountInFile[TESTFILEINDEX]++;
                    continue;
                }
            }
            sharedStruct->wordCountInFile[TESTFILEINDEX]++;
            (*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX])+=word.length();
        }
    }

    outputFile.close();
    
    //finalize
    sharedStruct->taskCompleted[TESTFILEINDEX] = true;
    printf("\nThere are %ld words in %s.\n", sharedStruct->wordCountInFile[TESTFILEINDEX], sharedStruct->filePath[1]);
    
    return NULL;
}