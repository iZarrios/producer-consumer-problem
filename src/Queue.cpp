#include <Queue.h>
#include <bits/stdc++.h>
#include <msg.h>

using namespace std;

Queue::Queue() {
  head = -1;
  tail = 0;
  size = 0;
}

void Queue::clear() {
  size = 0;
  head = -1;
  tail = 0;
}

long long Queue::get_size() { return size; }

bool Queue::is_empty() { return size == 0; }

struct mymsg_buffer Queue::front() {
  if (head == -1) {
    cout << "Queue is empty " << endl;
    return {};
  }
  return data[head];
}

void Queue::push(struct mymsg_buffer elem) {
  if (size == SHARED_MEM_SIZE) {
    cout << "Queue is full " << endl;
    return;
  }

  if (head == -1) {
    head = 0;
  }

  data[tail] = elem;
  tail++;
  size++;
}

struct mymsg_buffer Queue::pop() {
  if (head == -1) {
    cout << "Queue is empty " << endl;
    return {};
  }
  struct mymsg_buffer temp = data[head];
  for (int i = 0; i < size; i++) {
    data[i] = data[i + 1];
  }
  tail--;
  size--;
  if (size == 0) {
    head = -1;
    tail = 0;
  }
  return temp;
}
