#include "vsodium_xchacha20poly1305_ietf.h"

#include <sodium.h>
#include "vsodium_call_checker.h"

//=======================================================================================
vsodium_xchacha20poly1305_ietf::vsodium_xchacha20poly1305_ietf( const vsodium_string& init )
{
    auto keylen = crypto_aead_xchacha20poly1305_ietf_KEYBYTES;
    auto nonlen = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;

    if (init.empty()) return;

    if (keylen + nonlen > init.size())
    {
        throw std::runtime_error(__func__);
    }

    key = init.mid( 0, keylen );
    nonce = init.mid( keylen, nonlen );
}
//=======================================================================================
/*
int crypto_aead_xchacha20poly1305_ietf_encrypt(unsigned char *c,
                                               unsigned long long *clen_p,
                                               const unsigned char *m,
                                               unsigned long long mlen,
                                               const unsigned char *ad,
                                               unsigned long long adlen,
                                               const unsigned char *nsec,
                                               const unsigned char *npub,
                                               const unsigned char *k)
*/
vsodium_string vsodium_xchacha20poly1305_ietf::encrypt( const vsodium_string& msg )
{
    auto abytes = crypto_aead_xchacha20poly1305_ietf_abytes();

    vsodium_string cip( msg.size() + abytes );
    unsigned long long reslen = 0;
    VSODIUM_CALL << crypto_aead_xchacha20poly1305_ietf_encrypt( cip.data(), &reslen,
                                                                msg.data(), msg.size(),
                                                                nullptr, 0,
                                                                nullptr,
                                                                nonce.data(),
                                                                key.data() );

    cip.decrement_size_to( reslen );
    return cip;
}
//=======================================================================================
vsodium_string
vsodium_xchacha20poly1305_ietf::decrypt( const vsodium_string &cip, bool *_ok )
{
    bool phony_ok;
    bool & ok = _ok ? *_ok : phony_ok;

    vsodium_string msg( cip.size() );
    unsigned long long mlen = 0;
    auto rcode = crypto_aead_xchacha20poly1305_ietf_decrypt( msg.data(), &mlen,
                                                             nullptr,
                                                             cip.data(), cip.size(),
                                                             nullptr, 0,
                                                             nonce.data(),
                                                             key.data() );
    ok = rcode == 0;
    if (!ok) return {};

    msg.decrement_size_to( mlen );
    return msg;
}
//=======================================================================================
