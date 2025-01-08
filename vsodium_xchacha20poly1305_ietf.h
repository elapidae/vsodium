#ifndef VSODIUM_XCHACHA20POLY1305_IETF_H
#define VSODIUM_XCHACHA20POLY1305_IETF_H

#include "vsodium_string.h"

class vsodium_xchacha20poly1305_ietf
{
public:
    static size_t init_size();
    vsodium_xchacha20poly1305_ietf( const vsodium_string& init = {} );

    vsodium_string encrypt( const vsodium_string& msg ) const;
    vsodium_string decrypt( const vsodium_string& msg, bool *ok = nullptr ) const;

    bool valid() const;

    static void test();
private:
    vsodium_string nonce, key;
};

#endif // VSODIUM_XCHACHA20POLY1305_IETF_H
