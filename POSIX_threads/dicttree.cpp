/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#include "dicttree.h"
#include "populatetree.h"
#include "countwords.h"
#include "EXEC_STATUS.h"

#include <stdio.h>
#include <exception>
#include <cstring>

// helper function to get character index of given char
inline int getCharacterIndex(char character) {
    int ascii_code = (int)character;
    if (ascii_code >= 97 && ascii_code <= 122) { // character is a-z lower case
        return ascii_code - 97; // subtract 97 to shift ascii code to 0-25
    } else if (ascii_code >= 65 && ascii_code <= 90) { // character is A-Z upper case
        return ascii_code - 65; // subtract 65 to shift ascii code to 0-25
    } else if (ascii_code == 39) { // character is '
        return 26; // 26 represents apostrophe character
    }
    throw std::exception(); // invalid character detected, throw an std::exception
}

bool DictNode::add(const char* remainingChars, void * VoidPtr) {
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;
    
    if (strlen(remainingChars) == 0) { // base case
        return true;
    }

    int index;
    try {
        index = getCharacterIndex(remainingChars[0]); // throw an std::exception if the character is invalid
    } catch (std::exception e) {
        return false;
    }
    remainingChars++;

    if (next[index] == nullptr) { // if the node doesn't exist
        if (strlen(remainingChars) == 0) { // next character is the final character of the word
            next[index] = new DictNode(true);
            (*sharedStruct->numOfCharsProcessedFromFile[0])++; //just for line feed char
            //printf("\nchar %d was processed\n", (*sharedStruct->numOfCharsProcessedFromFile[0]));
            sharedStruct->wordCountInFile[0]++;
        } else {
            next[index] = new DictNode(false);
        }
    }
    (*sharedStruct->numOfCharsProcessedFromFile[0])++;
    //printf("\nchar %d was processed\n", (*sharedStruct->numOfCharsProcessedFromFile[0]));
    return next[index]->add(remainingChars, sharedStruct);
}

DictNode* DictNode::findEndingNodeOfAStr(const char* remainingStr, const char* strBeingSearched) {
    if (strlen(remainingStr) == 0) { // base case
        return this;
    }
    int index;
    try {
        index = getCharacterIndex(remainingStr[0]); // throw an std::exception if there's somehow an error
    } catch (std::exception e) {
        return nullptr;
    }
    if (next[index] == nullptr) { // string is not contained in tree
        return nullptr;
    }

    remainingStr++;
    return next[index]->findEndingNodeOfAStr(remainingStr, strBeingSearched);
}

// helper recursive function to count number of words stemming from a node
void recursiveCountNodes(DictNode* node, int& count) {
    for (int i = 0; i < NUMOFCHARS; i++) {
        if (node->next[i] != nullptr) {
            if (node->next[i]->isWord) {
                count++;
            }
            recursiveCountNodes(node->next[i], count);
        }
    }
}

void DictNode::countWordsStartingFromANode(int& count) {
    count = isWord ? 1 : 0; // initialize count to 1 or 0 depending on if word is in dictionary
    recursiveCountNodes(this, count);
}