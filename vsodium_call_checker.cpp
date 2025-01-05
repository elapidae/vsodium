#include "vsodium_call_checker.h"

#include <stdexcept>

//=======================================================================================
void vsodium_call_checker::operator <<(int rcode) const
{
    if (rcode == 0) return;
    throw std::runtime_error( std::string("vsodium: ") + info.data() );
}
//=======================================================================================
vsodium_call_checker& vsodium_call_checker::operator <<(std::string_view _info)
{
    info = _info;
    return *this;
}
//=======================================================================================
