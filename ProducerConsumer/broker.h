#ifndef BROKER_H
#define BROKER_H
/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/

#include <queue>
#include <semaphore.h>

#include "request.h"
#include "ridesharing.h"

struct Broker{

    int productionLimit;
    int currRequestsProduced;
    int currProduced;
    int currConsumed;

    //# of Human/Robot Requests currently in queue. [0] = human, [1] = robot
    int inRequestQueue[RequestTypeN];
    //# of Human/Robot Requests in total. [0] = human, [1] = robot
    int produced[RequestTypeN];
    //# of Requests consumed by each Consumer for each Request Type
    int* consumed[ConsumerTypeN];

    //SEMAPHORES
    /*MUTEX: exclusive access to buffer
    -accessing entity waits and then posts*/
    sem_t* Mutex;

    /*UNCONSUMED: # of requests left in buffer
    -consumer waits, producer posts*/
    sem_t* Unconsumed;

    /*AVAILABLE HUMAN SLOTS: # of human slots available
    -human producer waits, consumer posts*/
    sem_t* AvailableHumanSlots;

    /*AVAILABLE SLOTS: # of slots available
    -producer waits, consumer posts*/
    sem_t* AvailableSlots;

    /*BARRIER: prevent main from exiting before consumers*/
    sem_t* BarrierCostAlgo;

    /*BARRIERROBO: wait for Robo consumer to finish*/
    sem_t* BarrierFastAlgo;

    //BUFFER
    std::queue<Request*> buffer;

};

#endif