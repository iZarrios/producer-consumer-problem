#include "msg.h"
#include <iostream>
#include <queue>

void my_log_msg(char *commodity, struct tm tm_now, time_t now, std::string custom_message)
{
    char time_buffer[400];
    localtime_r(&now, &tm_now);
    strftime(time_buffer, sizeof(time_buffer), "[%Y/%m/%d %H:%M:%S]", &tm_now);

    std::cerr<<time_buffer<<" "<<commodity<<": "<<custom_message<<std::endl;

    /* printf("%s %s: %s\n", time_buffer, commodity, custom_message.c_str()); */
}
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
