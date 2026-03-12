#ifndef VSODIUM_EMODJI_H
#define VSODIUM_EMODJI_H

#include <stdint.h>
#include <string>


class vsodium_emodji
{
public:
    static std::string get_emodji_for( uint16_t word );

    static std::string work_emodji_v1( uint16_t word );
    static uint16_t work_emodji_v1_size();

    static std::string make_emodji_for_data( std::string data, uint8_t size = 4);

    static void test();
};

#endif // VSODIUM_EMODJI_H
