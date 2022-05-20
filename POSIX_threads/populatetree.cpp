/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#include "populatetree.h"
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

void * populate_tree_spawn(void * VoidPtr) {
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;
    
    //initialize what is going to be incremented
    (*sharedStruct->numOfCharsProcessedFromFile[DICTSRCFILEINDEX]) = (long) 0;
    sharedStruct->wordCountInFile[DICTSRCFILEINDEX] = 0;
    
     // stream to first parameter
    std::ifstream dictStream(sharedStruct->filePath[DICTSRCFILEINDEX]);
    std::string line;

    //parse through dictionary
    while (std::getline(dictStream, line)) {
        sharedStruct->dictRootNode->add(line.c_str(), sharedStruct);
    }
    
    return NULL;
}
