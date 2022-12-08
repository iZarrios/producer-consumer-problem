#include "msg.h"
#include <bits/stdc++.h>

/* #define SHARED_MEM_NAME "shared_mem" */

Msg::Msg(char commodity[], double price)
{
    /* this->commodity = commodity; */
    strcpy(this->commodity, commodity);
    this->price = price;
}
Msg::Msg()
{
}

std::string Msg::get_msg()
{
    char buffer[100];
    sprintf(buffer, "%s:%.3f", this->commodity, this->price);
    return buffer;
}
