#ifndef REQUEST_H
#define REQUEST_H

/*
CS 480 Operating Systems Assignment #4

NAME: ERICK HERNANDEZ
RED ID: 821321274

NAME: KAVON CACHO
RED ID: 822794135
*/
#include "ridesharing.h"

using namespace std;

class Request {
    public:
    RequestType RID; //request id
    inline Request(RequestType type);
};

//constructor
inline Request::Request(RequestType type) {
    RID = type;
}

#endif