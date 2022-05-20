#ifndef CONSUMER_H
#define CONSUMER_H
/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/
#include "broker.h"
#include "ridesharing.h"

/*
- Struct that will be instantiated for every Consumer
- Has a sleep time and consumer type as both will be different between both instances
*/
struct Consumer{
    int sleep_time;
    ConsumerType consumerType;
    struct Broker* broker;
};

#endif