#ifndef PRODUCER_H
#define PRODUCER_H
/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/
#include "ridesharing.h"
#include "broker.h"

/*
- Struct that will be instantiated for every Producer
- Has a sleep time and producer type as both will be different between both instances
*/
struct Producer{
    int sleep_time;
    struct Broker* broker;
    RequestType producerType;

};

#endif