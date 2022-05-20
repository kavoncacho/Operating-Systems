#ifndef CONSUMER_PRODUCER_FUNCTIONS_H
#define CONSUMER_PRODUCER_FUNCTIONS_H
/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/
#include "consumer.h"
#include "producer.h"
#include "ridesharing.h"
#include "request.h"

void insertRequest (Producer* producer);
RequestType removeRequest (Consumer* consumer); //returns id of what was removed
void* consumer_function(void * VoidPtr);
void* producer_function(void * VoidPtr);

#endif