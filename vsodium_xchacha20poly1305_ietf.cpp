#include "vsodium_xchacha20poly1305_ietf.h"

#include <sodium.h>
#include "vsodium_call_checker.h"
#include "vlog.h"

static constexpr auto keylen = crypto_aead_xchacha20poly1305_ietf_KEYBYTES;
static constexpr auto nonlen = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
static constexpr auto abytes = crypto_aead_xchacha20poly1305_ietf_ABYTES;

//=======================================================================================
size_t vsodium_xchacha20poly1305_ietf::init_size()
{
    return keylen + nonlen;
}
//=======================================================================================
vsodium_xchacha20poly1305_ietf::vsodium_xchacha20poly1305_ietf
                                                        ( const vsodium_string& init )
{
    if ( init.empty() ) return;

    if ( keylen + nonlen > init.size() )
    {
        throw std::runtime_error(__func__);
    }

    key = init.mid( 0, keylen );
    nonce = init.mid( keylen, nonlen );
}
//=======================================================================================
vsodium_string vsodium_xchacha20poly1305_ietf::encrypt( const vsodium_string& msg ) const
{
    if ( !valid() )
    {
        throw std::runtime_error("vsodium_xchacha20poly1305_ietf::encrypt");
    }

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
vsodium_xchacha20poly1305_ietf::decrypt( const vsodium_string &cip, bool *_ok ) const
{
    bool phony_ok;
    bool & ok = _ok ? *_ok : phony_ok;

    if ( !valid() )
    {
        ok = false;
        return {};
    }

    vsodium_string msg( cip.size() - abytes );
    unsigned long long mlen = 0;
    auto rcode = crypto_aead_xchacha20poly1305_ietf_decrypt( msg.data(), &mlen,
                                                             nullptr,
                                                             cip.data(), cip.size(),
                                                             nullptr, 0,
                                                             nonce.data(),
                                                             key.data() );
    ok = rcode == 0;
    if (!ok) return {};
    if (mlen > msg.size()) throw std::runtime_error(__func__);

    msg.decrement_size_to( mlen );
    return msg;
}
//=======================================================================================
bool vsodium_xchacha20poly1305_ietf::valid() const
{
    return key.size() == keylen &&
            nonce.size() == nonlen;
}
//=======================================================================================


//=======================================================================================
void vsodium_xchacha20poly1305_ietf::test()
{
    vsodium_string init;
    init.random(vsodium_xchacha20poly1305_ietf::init_size());

    vsodium_xchacha20poly1305_ietf aes(init);

    auto msg = "message1 message1 message1 message1 message1 message1 message1 message1";

    auto e1 = aes.encrypt( vsodium_string(msg) );
    auto e2 = aes.encrypt( vsodium_string(msg) );
    if (e1 != e2) throw std::runtime_error(__func__);
    vdeb << e1.to_hex() << e2.to_hex();
    vdeb << aes.decrypt(e1, nullptr);
}
//=======================================================================================
