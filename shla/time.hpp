#ifndef __SHLA_TIME_INCLUDED
#define __SHLA_TIME_INCLUDED

#include <cstdint>

namespace shla
{
    const char* month_names[] =
    {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    const char* day_of_week_names[] =
    {
        "Saturday",
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday"
    };

    /*
        Based on Christian Zeller's congruence.
        https://en.wikipedia.org/wiki/Zeller%27s_congruence
    */
    uint8_t GetDayOfWeek(uint16_t year, uint8_t month, uint8_t day_of_month)
    {
        if (month == 0 || month > 14)
        {
            return -1;
        }

        uint8_t K, J;

        if (month < 3)
        {
            month += 12;
        }

        K = static_cast<uint8_t>(year % 100);
        J = static_cast<uint8_t>(year / 100);

        return (day_of_month + ((13 * (month + 1)) / 5) + K + (K / 4) + (J / 4) - (J * 2)) % 7;
    }
}

#endif
