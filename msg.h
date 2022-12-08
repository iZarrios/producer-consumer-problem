#include <string>
#include <vector>

#pragma once

#define SHARED_MEM_NAME "shared_mem"
#define PERMS 0644

class Msg
{
  public:
    int m_type;
    char commodity[10];
    double price;

    Msg();
    Msg(char commodity[], double price);
    std::string get_msg();
};
