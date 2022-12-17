#include "msg.h"
#include <cstdlib>

void my_log_msg(char *commodity, struct tm tm_now, time_t t_now, std::string custom_message)
{
    char time_buffer[400];
    localtime_r(&t_now, &tm_now);
    strftime(time_buffer, sizeof(time_buffer), "[%d/%m/%Y %T", &tm_now);

    auto now = std::chrono::system_clock::now();

    const std::chrono::microseconds now_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::cerr << RED;
    std::cerr << "[" << time_buffer << "." << std::setfill('0') << std::setw(3) << now_ms.count() / 1000 << "] "
              << commodity << ": " << custom_message << std::endl;
    std::cerr << RESET;
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

SharedMemory::SharedMemory(int N)
{
    this->N = N;
    this->data = new struct mymsg_buffer[this->N];
    /* this->data = (struct mymsg_buffer *)std::malloc(sizeof(struct mymsg_buffer) * this->N); */
}
