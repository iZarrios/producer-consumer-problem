#include "msg.h"

// SRC: https://gist.github.com/bschlinker/844a88c09dcf7a61f6a8df1e52af7730

std::string get_time_stamp()
{

    // get a precise timestamp as a string

    const auto now = std::chrono::system_clock::now();
    const auto now_time_t = std::chrono::system_clock::to_time_t(now);
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream nowSs;
    nowSs << std::put_time(std::localtime(&now_time_t), "%d/%m/%Y %T") << '.' << std::setfill('0') << std::setw(3)
          << now_ms.count();

    return nowSs.str();
}

void my_log_msg(char *commodity, struct tm tm_now, time_t now, std::string custom_message)
{
    char time_buffer[400];
    localtime_r(&now, &tm_now);
    strftime(time_buffer, sizeof(time_buffer), "[%Y/%m/%d %T]", &tm_now);

    std::cout<<RED;
    std::cerr << "[" << get_time_stamp() << "] " << commodity << ": " << custom_message << std::endl;
    std::cout<<RESET;
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
