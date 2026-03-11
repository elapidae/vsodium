#pragma once

#include "vsodium_string.h"

//=======================================================================================
class vsodium_xchacha20poly1305_ietf
{
public:
    static size_t init_size();
    vsodium_xchacha20poly1305_ietf( const vsodium_string& init = {} );

    template<typename Char>
    vsodium_string encrypt( const Char* data, size_t size ) const
    {
        return encrypt( vsodium_string(data, size) );
    }

    template<typename AnyDataSize>
    vsodium_string encrypt( const AnyDataSize& msg ) const
    {
        return encrypt( vsodium_string(msg.data(), msg.size()) );
    }

    vsodium_string encrypt( const vsodium_string& msg ) const;
    vsodium_string decrypt( const vsodium_string& msg, bool *ok = nullptr ) const;

    bool valid() const;

    static void test();
private:
    vsodium_string nonce, key;
};
//=======================================================================================
