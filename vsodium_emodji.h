#ifndef VSODIUM_EMODJI_H
#define VSODIUM_EMODJI_H

#include <stdint.h>
#include <string>


class vsodium_emodji
{
public:
    static std::string get_emodji_for( uint16_t word );

    static void test();
};

#endif // VSODIUM_EMODJI_H
