/**
* Shared constants and data structures among threads
**/

/*
notes:
size of dict is 1127770
num of chars processed in dict 1127770
num of total words in dict 116687


*/

#define NUMOFFILES 2
#define DICTSRCFILEINDEX 0
#define TESTFILEINDEX 1
/* default number of progress marks for representing 100% progress */
#define DEFAULT_NUMOF_MARKS 50
/* minimum number of progress marks for representing 100% progress */
#define MIN_NUMOF_MARKS 10
/* place hash marks in the progress bar every N marks */
#define DEFAULT_HASHMARKINTERVAL 10
/* default minimum number of dictionary words starting from a prefix for printing or
writing to the output */
#define DEFAULT_MINNUM_OFWORDS_WITHAPREFIX 1
/* Common data shared between threads */
typedef struct {
    /**
    * root node of the dictionary tree
    */
    DictNode* dictRootNode;
    /**
    * parameters for printing progress bar
    */
    int numOfProgressMarks;

    int hashmarkInterval;
    /**
    * print a word and its count to the output file only if the
    * number of dictionary words starting from the word is equal to or
    * greater than this number
    */
    int minNumOfWordsWithAPrefixForPrinting;

    /**
    * filePath[0] - file path for the dictionary vocabulary file
    * providing words to populate the dictionary tree
    * filePath[1] - file path for the test source file
    * providing words to be used for testing
    */
    char *filePath[NUMOFFILES];

    /** store total number of characters in files, total word count in files
    * totalNumOfCharsInFile[DICTSRCFILEINDEX]
    * - number of total chars in the dictionary vocabulary file.
    * use stat, lstat, or fstat system call
    * totalNumOfCharsInFile[TESTFILEINDEX]
    * - number of total chars in the test file
    *
    * Hints: see man stat, lstat, or fstat to get the total number of bytes of the file
    * stat or lstat uses a filename, whereas fstat requires a file descriptor from a
    * low-level I/O call: e.g. open. If you are using high-level I/O, either use stat
    * (or lstat), or open the file first with the low-level I/O, then call fstat,
    * then close it.)
    *
    * Important: assume all text files are in Unix text file format, meaning, the end
    * of each line only has a line feed character. The stat, lstat, fstat would include the
    * count of the line feed character from each line.
    */
    long totalNumOfCharsInFile[NUMOFFILES];
    /**
    * Use numOfCharsProcessedFromFile to track ongoing progress of
    * number of characters read in from files and the subsequent thread logic.
    * We will refer to the long integer to which this variable dereferences as the
    * progress indicator.
    *
    * This progress indicator is updated by worker threads, and used by the main 
    * thread to display the progress for tracking the execution of the worker threads
    *
    * Important: this number can be incremented by the number of characters in
    * the line that is being read and processed, plus one to include the
    * line feed character at the end of each line of a Unix text file.
    * Do NOT convert the text files to a Windows DOS format.
    *
    * File is read in line by line, for each line read in:
    * 1) tokenize the line by delimiters to a word array, with each separated word:
    * insert the word to the dictionary tree or search, count, and print the word
    * in the dictionary tree, and then increment the word count of the file:
    * wordCountInFile (see below)
    *
    * 2) update the numOfCharsProcessedFromFile by incrementing it by the
    * number of characters in the line, plus one to include the line feed
    * character at the end of each line of a Unix text file.
    *
    * numOfCharsProcessedFromFile[DICTSRCFILEINDEX]
    * - number of chars read in and processed from
    * the dictionary vocabulary file
    * numOfCharsProcessedFromFile[TESTFILEINDEX]
    * - number of chars read in and processed from
    * the test file
    */
    long *numOfCharsProcessedFromFile[NUMOFFILES];
    /**
    * wordCountInFile[DICTSRCFILEINDEX]
    * - number of total words in the dictionary vocabulary file.
    * wordCountInFile[TESTFILEINDEX]
    * - number of total words in the test file
    */
    long wordCountInFile[NUMOFFILES];
    /**
    * completed flags indicate the completion of the thread logic, one
    * for the populatetree thread, one for countwords thread
    * Important: the completed flag of populatetree thread may be
    * used to force the precedence constraint that the populatetree
    * thread logic must be executed before the countwords thread
    *
    * taskCompleted[DICTSRCFILEINDEX] 
    * - boolean flag to indicate whether the tree population
    * thread has completed the task: read words from the
    * dictionary source file and populate the tree
    *
    * important: you may want to set the completed flag for the
    * populatetree thread to true only after the main thread fully displays
    * the progress bar upon the completion of the thread logic
    *
    * taskCompleted[TESTFILEINDEX]
    * - boolean flag to indicate whether the counting words
    * thread has completed the task for reading and processing all words from the
    * test file.
    */
    bool taskCompleted[NUMOFFILES];

} EXEC_STATUS;