#ifndef VSODIUM_KEY_EXCHANGE_H
#define VSODIUM_KEY_EXCHANGE_H

#include "vsodium_string.h"

class vsodium_key_exchange
{
public:
    struct kx
    {
        kx();
        vsodium_string rx, tx;
    };

    vsodium_key_exchange();

    const vsodium_string& pub_key();

    kx shared_client( const vsodium_string& other_pub_key, bool *ok );
    kx shared_server( const vsodium_string& other_pub_key, bool *ok );

private:
    vsodium_string pk, sk;
};

#endif // VSODIUM_KEY_EXCHANGE_H
