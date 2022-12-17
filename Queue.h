#pragma once

#include "msg.h"

class Queue
{
    // for another problem we may need to reallocate
    // if more than SHARED_MEM_SIZE is needed just like the usual stl
    // implementation we can just doulbe the initial array size
    struct mymsg_buffer data[SHARED_MEM_SIZE];
    long long head;
    long long tail;
    long long size;

  public:
    Queue();
    long long get_size();
    bool is_empty();
    void clear();
    struct mymsg_buffer front();
    void push(struct mymsg_buffer);
    struct mymsg_buffer pop();
};
