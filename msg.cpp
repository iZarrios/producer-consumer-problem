#include "msg.h"
#include <queue>

Commidity::Commidity()
{
}
Commidity::Commidity(std::string name, double price)
{
    this->name = name;
    this->price = price;

    this->last_prices.push(price);
    this->price_state = UNCHANGED;
    this->avg_price_state = UNCHANGED;
}
