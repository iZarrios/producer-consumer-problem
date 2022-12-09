#pragma once

template <typename T>

class Queue
{
    T *data;
    int head;
    int tail;
    int MAX_SIZE;
    int current_size;

  public:
    Queue(int);
    bool is_empty();
    int size();
    void push(T);
    void pop();
    T front();
};
/* custom_queue(int size) */
/* { */
/*     MAX_SIZE = size; */
/*     this->data = new T[MAX_SIZE]; */
/*     this->head = -1; */
/*     this->tail = 0; */
/*     this->size = 0; */
