#include "Queue.h"

#include <bits/stdc++.h>

using namespace std;

Queue::Queue()
{
    data = new Commidity *[MAX_SIZE];
    /* for (int i = 0; i < MAX_SIZE; i++) */
    /* { */
    /*     data[i] = nullptr; */
    /* } */

    head = -1;
    tail = 0;
    size = 0;
}

int Queue::getSize()
{
    return size;
}

bool Queue::isEmpty()
{
    return size == 0;
}

Commidity *Queue::front()
{
    if (head == -1)
    {
        cout << "Queue is empty " << endl;
        return 0;
    }
    return data[head];
}

void Queue::push(Commidity *elem)
{
    if (size == MAX_SIZE)
    {
        cout << "Queue is full " << endl;
        return;
    }

    if (head == -1)
    {
        head = 0;
    }

    data[tail] = elem;
    tail++;
    size++;
}

Commidity *Queue::pop()
{
    if (head == -1)
    {
        cout << "Queue is empty " << endl;
        return 0;
    }
    Commidity *temp = data[head];
    for (int i = 0; i < size; i++)
    {
        data[i] = data[i + 1];
    }
    tail--;
    size--;
    if (size == 0)
    {
        head = -1;
        tail = 0;
    }
    return temp;
}
