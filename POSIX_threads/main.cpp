/*
    Kavon Cacho
    RedID: 822794135
    CS480 Section 3
*/

#include "dicttree.h"
#include "populatetree.h"
#include "countwords.h"
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
#include <sys/stat.h>
#include <cmath>
#include <ostream>

//helper method for populate progress bar
void * populateProgressBar(void * VoidPtr, char progressBar[], int progressBarSize) {
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;
    
    //determines how many characters that is represented by a single progress mark
    double populateProgress = sharedStruct->totalNumOfCharsInFile[DICTSRCFILEINDEX] / sharedStruct->numOfProgressMarks;

    for (int i = 0; i < progressBarSize; ++i) {
        while ((double) (*sharedStruct->numOfCharsProcessedFromFile[DICTSRCFILEINDEX]) < (populateProgress * (double) (i + 1))) {
            /*
            This while loop solely serves the purpose of stalling the progress bar
            print until the number of chars processed reaches a "checkpoint".
            */
        }
        std::cout << progressBar[i] << std::flush;
    }

    if (!sharedStruct->taskCompleted[DICTSRCFILEINDEX]) {
        sharedStruct->taskCompleted[DICTSRCFILEINDEX] = true;
        std::cout << "\n" << "There are " << sharedStruct->wordCountInFile[DICTSRCFILEINDEX] << " words in " << sharedStruct->filePath[DICTSRCFILEINDEX] << "." << "\n" << std::flush;
    }
    
    return NULL;
}

//helper method for countwords progress bar
void * countProgressBar(void * VoidPtr, char progressBar[], int progressBarSize) {
    EXEC_STATUS* sharedStruct;
    sharedStruct = (EXEC_STATUS*) VoidPtr;
    
    //determines how many characters that is represented by a single progress mark
    double countwordsProgress = sharedStruct->totalNumOfCharsInFile[TESTFILEINDEX] / sharedStruct->numOfProgressMarks;

    while (sharedStruct->taskCompleted[DICTSRCFILEINDEX] == false) {
        //while loop to stall until first task is done
    }

    for (int i = 0; i < progressBarSize; ++i) {
        while (((double)(*sharedStruct->numOfCharsProcessedFromFile[TESTFILEINDEX]) < (countwordsProgress * (double) (i + 1)))) {
            /*
            This while loop solely serves the purpose of stalling the progress bar
            print until the number of chars processed reaches a "checkpoint".
            */
        }
        std::cout << progressBar[i] << std::flush;
    }
    
    return NULL;
}

int main(int argc, char** argv) {
    
    //parameter error check
    if ((argc < 3) || (argc > 9)) {
        printf("\nPlease use the proper amount of parameters.\n\n");
        return -1;
    }

    //file existence check
    for (int i = 1; i <= 2; i++) {
        std::ifstream fileCheck(argv[i]);
        if (!fileCheck.is_open()) {
            printf("\nUnable to open <<%s>>\n\n", argv[i]);
            return -1;
        }
        fileCheck.close();
    }

    
    //thread declaration
    pthread_t populateTreeThread, countWordsThread;
    
    //structure delcaration and initialization
    DictNode* root = new DictNode(false);
    EXEC_STATUS* sharedData = new EXEC_STATUS();
    
    //struct data initialization
    sharedData->dictRootNode = root;
    sharedData->numOfProgressMarks = DEFAULT_NUMOF_MARKS;
    sharedData->hashmarkInterval = DEFAULT_HASHMARKINTERVAL;
    sharedData->minNumOfWordsWithAPrefixForPrinting = DEFAULT_MINNUM_OFWORDS_WITHAPREFIX;
    sharedData->filePath[DICTSRCFILEINDEX] = (char*) argv[1];
    sharedData->filePath[TESTFILEINDEX] = (char*) argv[2];
    sharedData->numOfCharsProcessedFromFile[DICTSRCFILEINDEX] = new long;
    sharedData->numOfCharsProcessedFromFile[TESTFILEINDEX] = new long;
    sharedData->taskCompleted[DICTSRCFILEINDEX] = false;
    sharedData->taskCompleted[TESTFILEINDEX] = false;

    //get file sizes
    struct stat stat1;
    struct stat stat2;
    if ((stat(sharedData->filePath[DICTSRCFILEINDEX], &stat1) == 0) && (stat(sharedData->filePath[TESTFILEINDEX], &stat2) == 0)) {
        sharedData->totalNumOfCharsInFile[DICTSRCFILEINDEX] = stat1.st_size;
        sharedData->totalNumOfCharsInFile[TESTFILEINDEX] = stat2.st_size;
    }

    //optional flag checking
    int flags;
    while ((flags = getopt(argc, argv, "p:h:n:")) != -1) {
        switch (flags) {
            case 'p':
                if (atoi(optarg) >= 10) {
                    sharedData->numOfProgressMarks = atoi(optarg);
                    break;
                } else {
                    printf("\nNumber of progress marks must be a number and at least 10. \n\n");
                    return -1;
                }
                
            case 'h':
                if ((atoi(optarg) > 0) && (atoi(optarg) <= 10)) {
                    sharedData->hashmarkInterval = atoi(optarg);
                    break;
                } else {
                    printf("\nHash mark interval for progress must be a number, greater than 0, and less than or equal to 10. \n\n");
                    return -1;
                }
            case 'n':
                sharedData->minNumOfWordsWithAPrefixForPrinting = atoi(optarg);
                break;
            default:
                printf("\nSomething messed up.\n\n");
                return -1;
        }
    }

    //create progress bar array
    //array that represents the whole bar when printed fully
    char progressBar[sharedData->numOfProgressMarks];
    int interval = 1;
    for (int i = 0; i < sharedData->numOfProgressMarks; ++i) {
        if (interval == sharedData->hashmarkInterval) {
            progressBar[i] = '#';
            interval = 1;
        } else {
            progressBar[i] = '-';
            interval++;
        }
        
    }

    //GOOOOOOO
    pthread_create(&populateTreeThread, NULL, &populate_tree_spawn, (void *) sharedData);
    pthread_create(&countWordsThread, NULL, &count_words_spawn, (void *) sharedData);
    populateProgressBar((void *) sharedData, progressBar, sharedData->numOfProgressMarks);
    countProgressBar((void *) sharedData, progressBar, sharedData->numOfProgressMarks);
    
    pthread_exit(NULL);
    return 0;
}