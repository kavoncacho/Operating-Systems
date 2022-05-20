/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/

#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#include "consumer.h"
#include "producer.h"
#include "request.h"
#include "io.h"

#define MSPERSEC 1000
#define NSPERMS 1000000

/*
- Function for inserting produced requests into the buffer (queue).
- Takes in the producer (so it has access to the buffer 
and request type).
- Instantiate a new request, push onto the buffer, update current and totals,
and print.
*/
void insertRequest (Producer* producer){
    Request* request = new Request(producer->producerType);
    //push request to buffer
    producer->broker->buffer.push(request);
    //increment # of requests in buffer
    producer->broker->inRequestQueue[producer->producerType]++;
    //increment # of requests produced
    producer->broker->produced[producer->producerType]++;
    producer->broker->currProduced++;
    //IO ADD CALL
    io_add_type(producer->producerType, producer->broker->inRequestQueue, producer->broker->produced);
}

/*
-Function to remove requests from buffer
INPUT: Consumer pointer
OUTPUT: int RID, representing request type that was removed*/
RequestType removeRequest (Consumer* consumer) {
    Request* popped;
    RequestType rid;
    //get Request* to be popped
    popped = consumer->broker->buffer.front();
    //pop next element
    consumer->broker->buffer.pop();
    //decrement # of RID requests in buffer
    consumer->broker->inRequestQueue[popped->RID]--;
    //increment # RID requests consumed by consumerType algo
    consumer->broker->consumed[consumer->consumerType][popped->RID]++;
    consumer->broker->currConsumed++;
    //IO REMOVE CALL
    io_remove_type(consumer->consumerType, popped->RID, consumer->broker->inRequestQueue, consumer->broker->consumed[consumer->consumerType]);
    rid = popped->RID;
    //cleanup memory of removed Request*
    delete(popped);
    return rid;
}

/*
- Kickoff function for both consumer threads
- Consumes requests of either type from the broker
*/
void* consumer_function(void * VoidPtr){
    Consumer* consumer;
    consumer = (Consumer*) VoidPtr;

    struct timespec SleepTime;
    SleepTime.tv_sec = consumer->sleep_time / MSPERSEC;
    SleepTime.tv_nsec = (consumer->sleep_time % MSPERSEC) * NSPERMS;

    while(true){
        RequestType popped;
        //stops process if 0 requests in buffer
        sem_wait(consumer->broker->Unconsumed);

        /*ENTER CRITICAL SECTION*/
        sem_wait(consumer->broker->Mutex);
        //check if max requests consumed
        if(consumer->broker->currConsumed >= consumer->broker->productionLimit){
            //release mutex lock and allow other processes to break loop
            sem_post(consumer->broker->Mutex);
            break;
        }
        popped = removeRequest(consumer);
        /*EXIT CRITICAL SECTION*/
        sem_post(consumer->broker->Mutex);
        
        //check if removed request is Human
        if(popped == HumanDriver){
            //signal available human slot
            sem_post(consumer->broker->AvailableHumanSlots);
        }
        //signal available slot
        sem_post(consumer->broker->AvailableSlots);
        //simulate consumption of request
        nanosleep(&SleepTime, NULL);
    }
    //signal consumer thread completion
    if(consumer->consumerType == CostAlgoDispatch){
        sem_post(consumer->broker->BarrierCostAlgo);
    }
    else{
        sem_post(consumer->broker->BarrierFastAlgo);
    }
    pthread_exit;

}

/*
- Kickoff function for both producer threads
- Pushes new requests onto the broker based on type
*/
void* producer_function(void * VoidPtr){
    Producer* producer;
    producer = (Producer*) VoidPtr;

    struct timespec SleepTime;
    SleepTime.tv_sec = producer->sleep_time / MSPERSEC;
    SleepTime.tv_nsec = (producer->sleep_time % MSPERSEC) * NSPERMS;
    
    while (true) {
        nanosleep(&SleepTime, NULL);
    
        //check if human request producer
        if (producer->producerType == HumanDriver) {
            //stops process if buffer as max humans in buffer
            sem_wait(producer->broker->AvailableHumanSlots);
        }
        //stops process if buffer is full
        sem_wait(producer->broker->AvailableSlots);
        
        /*ENTER CRITICAL SECTION*/
        sem_wait(producer->broker->Mutex);
        //check if max produced
        if(producer->broker->currProduced >= producer->broker->productionLimit){
            //allow pending processes to enter the critical section, to break their own loop
            sem_post(producer->broker->Mutex);
            //allow pending consumer to get to critical section and break loop
            sem_post(producer->broker->Unconsumed);
            break;
        }

        insertRequest(producer);

        /*EXIT CRITICAL SECTION*/
        sem_post(producer->broker->Mutex);
        //inform consumer of unconsumed request
        sem_post(producer->broker->Unconsumed);
    }
    pthread_exit;
}