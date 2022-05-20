/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/
#include <unistd.h> //getopt
#include <stdlib.h> //exit
#include <stdio.h>  //printf
#include <locale>   //isdigit
#include <pthread.h>    //pthreads
#include <semaphore.h>  //semaphores
#include <queue> //queue

#include "producer.h"
#include "consumer.h"
#include "broker.h"
#include "ridesharing.h"
#include "consumer_producer_functions.h"
#include "io.h"

//default argument values
#define DEFAULT_TOTAL_REQUESTS 120
#define DEFAULT_COST_SAVING_MS 0
#define DEFAULT_FAST_MATCHING_MS 0
#define DEFAULT_HUMAN_REQ_MS 0
#define DEFAULT_AUTO_REQ_MS 0

//Max for human requests
#define MAX_HUMAN_REQUESTS 4
#define MAX_REQUESTS 12

//SEMAPHORE INITIALIZATION VALS
#define MUTEX 0
#define UNCONSUMED 0
#define MAIN_BARRIER 0


//method headers
void getArguments(int argc, char* argv[], int& getN, int& getC, int& getF, int& getH, int& getA);

int main(int argc, char **argv) {
    
    int getN;
    int getC;
    int getF;
    int getH;
    int getA;

    /*GET ARGUMENTS*/
    getArguments(argc, argv, getN, getC, getF, getH, getA);

    /*STRUCTURE INITIALIZATION*/
    struct Broker* broke = new Broker();
    struct Producer* humanProducer = new Producer();
    struct Producer* robotProducer = new Producer();
    struct Consumer* fastConsumer = new Consumer();
    struct Consumer* costConsumer = new Consumer();
    
    /*BROKER VAR INITIALIZATION*/
    broke->productionLimit = getN;
    broke->currRequestsProduced = 0;
    broke->currProduced = 0;
    broke->currConsumed = 0;
    
    for (int i = 0; i < RequestTypeN; ++i) {
        broke->inRequestQueue[i] = 0;
        broke->produced[i] = 0;
        
    }
    for(int i = 0; i < ConsumerTypeN; ++i){
        broke->consumed[i] = new int[RequestTypeN]();
    }

    /*PRODUCER VAR INITIALIZATION*/
    humanProducer->broker = broke;
    robotProducer->broker = broke;
    humanProducer->sleep_time = getH;
    robotProducer->sleep_time = getA;
    humanProducer->producerType = HumanDriver;
    robotProducer->producerType = RoboDriver;
    
    /*CONSUMER VAR INITIALIZATION*/
    fastConsumer->broker = broke;
    costConsumer->broker = broke;
    fastConsumer->sleep_time = getF;
    costConsumer->sleep_time = getC;
    fastConsumer->consumerType = FastAlgoDispatch;
    costConsumer->consumerType = CostAlgoDispatch;
    
    /*SEMAPHORE INITIALIZATION*/
    broke->Mutex = new sem_t;
    broke->Unconsumed = new sem_t;
    broke->AvailableHumanSlots = new sem_t;
    broke->AvailableSlots = new sem_t;
    broke->BarrierCostAlgo = new sem_t;
    broke->BarrierFastAlgo = new sem_t;

    if (sem_init(broke->Mutex, 0, 1) == -1) {
        fprintf(stderr, "Unable to initialize Mutex semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(broke->Unconsumed, 0, UNCONSUMED) == -1) {
        fprintf(stderr, "Unable to initialize Unconsumed semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(broke->AvailableHumanSlots, 0, MAX_HUMAN_REQUESTS) == -1) {
        fprintf(stderr, "Unable to initialize AvailableHumanSlots semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(broke->AvailableSlots, 0, MAX_REQUESTS) == -1) {
        fprintf(stderr, "Unable to initialize AvailableSlots semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(broke->BarrierCostAlgo, 0, MAIN_BARRIER) == -1) {
        fprintf(stderr, "Unable to initialize BarrierCostAlgo semaphore\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(broke->BarrierFastAlgo, 0, MAIN_BARRIER) == -1) {
        fprintf(stderr, "Unable to initialize BarrierFastAlgo semaphore\n");
        exit(EXIT_FAILURE);
    }

    /*INTIALIZE THREADS*/
    pthread_attr_t pthread_attributes;
    pthread_t costConsumerThread;
    pthread_t fastConsumerThread;
    pthread_t humanProducerThread;
    pthread_t robotProducerThread;

    pthread_attr_init(&pthread_attributes);

    if (pthread_create(&humanProducerThread, &pthread_attributes, &producer_function, (void*) humanProducer)) {
        fprintf(stderr, "Unable to create Cost Algo Consumer thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&robotProducerThread, &pthread_attributes, &producer_function, (void*) robotProducer)) {
        fprintf(stderr, "Unable to create Cost Algo Consumer thread\n");
        exit(EXIT_FAILURE);
    }
    //kick off
    if (pthread_create(&costConsumerThread, &pthread_attributes, &consumer_function, (void*) costConsumer)) {
        fprintf(stderr, "Unable to create Cost Algo Consumer thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&fastConsumerThread, &pthread_attributes, &consumer_function, (void*) fastConsumer)) {
        fprintf(stderr, "Unable to create Fast Algo Consumer thread\n");
        exit(EXIT_FAILURE);
    }

    /*WAIT FOR CHILD THREADS TO FINISH*/
    sem_wait(broke->BarrierCostAlgo);
    sem_wait(broke->BarrierFastAlgo);

    //IO REPORT CALL
    io_production_report(broke->produced, broke->consumed);

    //CLEANUP
    delete(costConsumer);
    delete(fastConsumer);
    delete(humanProducer);
    delete(robotProducer);
    delete(broke->Mutex);
    delete(broke->Unconsumed);
    delete(broke->AvailableHumanSlots);
    delete(broke->AvailableSlots);
    delete(broke->consumed[CostAlgoDispatch]);
    delete(broke->consumed[FastAlgoDispatch]);

    return 0;

}

void getArguments(int argc, char* argv[], int& getN, int& getC, int& getF, int& getH, int& getA){
    //flags to prevent duplicate flag uses
    bool nFlag = false;
    bool cFlag = false;
    bool fFlag = false;
    bool hFlag = false;
    bool aFlag = false;

    //set argument variables to default values 
    getN = DEFAULT_TOTAL_REQUESTS;
    getC = DEFAULT_COST_SAVING_MS;
    getF = DEFAULT_FAST_MATCHING_MS;
    getH = DEFAULT_HUMAN_REQ_MS;
    getA = DEFAULT_AUTO_REQ_MS;

    //get command line arguments and change argument variables if necessary
    int Option;
    while((Option = getopt(argc, argv, "n:c:f:h:a:")) != -1){
        switch(Option){
            case 'n':
                if(nFlag){
                    printf("ERROR: Multiple -n flags");
                    exit(EXIT_FAILURE);
                }
                else if(atoi(optarg) < 0){
                    printf("Total number of requests must be a number, greater than or equal to 0");
                    exit(EXIT_FAILURE);
                }
                else{
                    getN = atoi(optarg);
                }
                nFlag = true;
                break;
            case 'c':
                if(cFlag){
                    printf("ERROR: Multiple -c flags");
                    exit(EXIT_FAILURE);
                }
                else if(atoi(optarg) < 0){
                    printf("Cost Saving Dispatcher milliseconds must be a number, greater than or equal to 0");
                    exit(EXIT_FAILURE);
                }
                else{
                    getC = atoi(optarg);
                }
                cFlag = true;
                break;
            case 'f':
                if(fFlag){
                    printf("ERROR: Multiple -f flags");
                    exit(EXIT_FAILURE);
                }
                else if(atoi(optarg) < 0){
                    printf("Fast Matching dispatcher milliseconds must be a number, greater than or equal to 0");
                    exit(EXIT_FAILURE);
                }
                else{
                    getF = atoi(optarg);
                }
                fFlag = true;
                break;
            case 'h':
                if(hFlag){
                    printf("ERROR: Multiple -h flags");
                    exit(EXIT_FAILURE);
                }
                else if(atoi(optarg) < 0){
                    printf("Human Driver Ride Request milliseconds must be a number, greater than or equal to 0");
                    exit(EXIT_FAILURE);
                }
                else{
                    getH = atoi(optarg);
                }
                hFlag = true;
                break;
            case 'a':
                if(aFlag){
                    printf("ERROR: Multiple -a flags");
                    exit(EXIT_FAILURE);
                }
                else if(atoi(optarg) < 0){
                    printf("Autonomous Driver Ride Request milliseconds must be a number, greater than or equal to 0");
                    exit(EXIT_FAILURE);
                }
                else{
                    getA = atoi(optarg);
                }
                aFlag = true;
                break;
            case '?':
                printf("ERROR: Unknown argument");
                exit(EXIT_FAILURE);
            default:
                printf("ERROR: Default");
                exit(EXIT_FAILURE);

        }
    }
}

