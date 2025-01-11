#ifndef VSODIUM_KEY_EXCHANGE_H
#define VSODIUM_KEY_EXCHANGE_H

#include "vsodium_string.h"

//=======================================================================================
class vsodium_key_exchange
{
public:
    struct kx final
    {
        kx();
        vsodium_string rx, tx;
    };
    static size_t public_keysize();

    vsodium_key_exchange();

    const vsodium_string& pub_key() const;

    kx shared_client( const vsodium_string& other_pub_key, bool *ok );
    kx shared_server( const vsodium_string& other_pub_key, bool *ok );

private:
    vsodium_string pk, sk;
};
//=======================================================================================

#endif // VSODIUM_KEY_EXCHANGE_H
