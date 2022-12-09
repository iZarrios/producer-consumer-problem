#pragma once

#include "msg.h"

#define MAX_SIZE 2048

/* typedef char T; */

class Queue
{
    Commidity **data;
    int head;
    int tail;
    int size;

  public:
    Queue();
    int getSize();
    bool isEmpty();
    Commidity *front();
    void push(Commidity *);
    Commidity *pop();
};
