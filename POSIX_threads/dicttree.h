/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#ifndef DICTTREE_H_
#define DICTTREE_H_

#include <cstdlib>

#define NUMOFCHARS 27 // a-z + '

class DictNode {
public:
    DictNode(bool isWord) : isWord(isWord), next() {}
    bool isWord;
    DictNode* next[NUMOFCHARS];

    bool add(const char* remainingChars, void * VoidPtr);
    DictNode* findEndingNodeOfAStr(const char* remainingStr, const char* strBeingSearched);
    void countWordsStartingFromANode(int& count);
};

#endif