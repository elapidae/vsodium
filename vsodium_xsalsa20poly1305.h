#ifndef VSODIUM_XSALSA20POLY1305_H
#define VSODIUM_XSALSA20POLY1305_H

#include "vsodium_string.h"


class vsodium_xsalsa20poly1305
{
public:
    vsodium_xsalsa20poly1305( const vsodium_string& init = {} );

    vsodium_string encrypt( const vsodium_string& msg ) const;
    vsodium_string decrypt( const vsodium_string& cip ) const;

private:
    vsodium_string nonce;
    vsodium_string key;
};

#endif // VSODIUM_XSALSA20POLY1305_H
