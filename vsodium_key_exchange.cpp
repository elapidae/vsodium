#include "vsodium_key_exchange.h"

#include <sodium.h>
#include "vsodium_call_checker.h"

//=======================================================================================
vsodium_key_exchange::vsodium_key_exchange()
{
    pk.resize(crypto_kx_PUBLICKEYBYTES);
    sk.resize(crypto_kx_SECRETKEYBYTES);
    crypto_kx_keypair( pk.data(), sk.data() );
}
//=======================================================================================
const vsodium_string& vsodium_key_exchange::pub_key()
{
    return pk;
}
//=======================================================================================
vsodium_key_exchange::kx
vsodium_key_exchange::shared_client( const vsodium_string & other_pub_key, bool *ok )
{
    vsodium_key_exchange::kx res;
    auto rcode = crypto_kx_client_session_keys( res.rx.data(), res.tx.data(),
                                                pk.data(), sk.data(),
                                                other_pub_key.data() );
    *ok = rcode == 0;
    return res;
}
//=======================================================================================
vsodium_key_exchange::kx
vsodium_key_exchange::shared_server( const vsodium_string & other_pub_key, bool *ok )
{
    vsodium_key_exchange::kx res;
    auto rcode = crypto_kx_server_session_keys( res.rx.data(), res.tx.data(),
                                                pk.data(), sk.data(),
                                                other_pub_key.data() );
    *ok = rcode == 0;
    return res;
}
//=======================================================================================
vsodium_key_exchange::kx::kx()
{
    tx.resize(crypto_kx_SESSIONKEYBYTES);
    rx.resize(crypto_kx_SESSIONKEYBYTES);
}
//=======================================================================================
