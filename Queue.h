#pragma once

#include "msg.h"


class Queue
{
    struct mymsg_buffer data[SHARED_MEM_SIZE];
    long long head;
    long long tail;
    long long size;

  public:
    Queue();
    long long getSize();
    bool isEmpty();
    void clear();
    struct mymsg_buffer front();
    void push(struct mymsg_buffer);
    struct mymsg_buffer pop();
};
