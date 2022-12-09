#include "msg.h"

Commidity::Commidity()
{
}
Commidity::Commidity(std::string name, double price, double avg_price)
{
    this->name = name;
    this->price = price;
    this->last_price = 0;
    this->price_state = UNCHANGED;
    this->avg_price = avg_price;
    this->last_avg_price = UNCHANGED;
}
