#include "vsodium_curve25519.h"

#include <iostream>
#include <sodium.h>
#include <fstream>
#include "vsodium_call_checker.h"

//=======================================================================================
_vsodium_key_::_vsodium_key_( const vsodium_string& k )
    : _key(k)
{}
//=======================================================================================
const vsodium_string &_vsodium_key_::key() const
{
    return _key;
}
//=======================================================================================


//=======================================================================================
void vsodium_ed25519::random()
{
    _key.resize(crypto_sign_ed25519_SECRETKEYBYTES);
    vsodium_string ed25519_pk(crypto_sign_ed25519_PUBLICKEYBYTES);
    VSODIUM_CALL << crypto_sign_ed25519_keypair( ed25519_pk.data(), _key.data() );

    auto rhs_pk = _key.right(crypto_sign_ed25519_PUBLICKEYBYTES);
    if ( rhs_pk != ed25519_pk )
    {
        throw std::runtime_error("25519 keys API changed");;
    }
}
//=======================================================================================
bool vsodium_ed25519::load( std::string fname )
{
    std::ifstream i;
    i.open(fname);
    if ( i.bad() ) return false;

    vsodium_string k(crypto_sign_ed25519_SECRETKEYBYTES);
    i.read(k.sdata(), crypto_sign_ed25519_SECRETKEYBYTES);
    if ( !i.good() ) return false;

    _key = k;
    return true;
}
//=======================================================================================
bool vsodium_ed25519::save(std::string fname)
{
    if (_key.empty()) return false;

    std::ofstream o;
    o.open(fname);
    o.write(_key.sdata(), _key.size());

    return o.good();
}
//=======================================================================================
vsodium_string vsodium_ed25519::sign( const vsodium_string& msg ) const
{
    auto smlen = crypto_sign_ed25519_bytes() + msg.size();
    vsodium_string sm(smlen);
    unsigned long long smlen_res;


    VSODIUM_CALL << crypto_sign_ed25519( sm.data(), &smlen_res,
                                         msg.data(), msg.size(),
                                         _key.data() );

    if (smlen != smlen_res) throw std::runtime_error(__func__);

    return sm;
}
//=======================================================================================
vsodium_string vsodium_ed25519::public_key::verify(const vsodium_string &sm, bool *_ok)
{
    bool phony_ok;
    bool &ok = _ok ? *_ok : phony_ok;

    if ( !valid() )
    {
        ok = false;
        return {};
    }

    vsodium_string msg( sm.size() - crypto_sign_ed25519_bytes() );
    unsigned long long mlen_res;

    auto rcode = crypto_sign_ed25519_open( msg.data(), &mlen_res,
                                           sm.data(), sm.size(),
                                           _key.data() );

    if (rcode != 0)
    {
        ok = false;
        return {};
    }

    if (mlen_res != msg.size()) throw std::runtime_error("memory kill");

    return msg;
}
//=======================================================================================
vsodium_ed25519::x25519_public_key vsodium_ed25519::public_key::x_public() const
{
    if (!valid()) throw std::runtime_error(__func__);

    vsodium_string pk(crypto_scalarmult_curve25519_BYTES);
    VSODIUM_CALL << crypto_sign_ed25519_pk_to_curve25519( pk.data(), key().data() );

    return x25519_public_key(pk);
}
//=======================================================================================
bool vsodium_ed25519::valid() const
{
    return _key.size() == crypto_sign_ed25519_SECRETKEYBYTES;
}
//=======================================================================================
vsodium_ed25519::public_key vsodium_ed25519::ed_public() const
{
    if (!valid()) throw std::runtime_error(__func__);

    vsodium_string ed25519_pk(crypto_sign_ed25519_PUBLICKEYBYTES);
    VSODIUM_CALL << crypto_sign_ed25519_sk_to_pk( ed25519_pk.data(), _key.data() );

    return public_key(ed25519_pk);
}
//=======================================================================================
vsodium_ed25519::x25519_secret_key vsodium_ed25519::x_secret() const
{
    if (!valid()) throw std::runtime_error(__func__);

    vsodium_string x25519_sk(crypto_scalarmult_curve25519_BYTES);
    VSODIUM_CALL << crypto_sign_ed25519_sk_to_curve25519(x25519_sk.data(), key().data());

    return x25519_secret_key(x25519_sk);
}
//=======================================================================================


//=======================================================================================
vsodium_ed25519::public_key::public_key( const vsodium_string& pk )
    : _vsodium_key_(pk)
{}
//=======================================================================================
bool vsodium_ed25519::public_key::valid() const
{
    return _key.size() == crypto_sign_ed25519_PUBLICKEYBYTES;
}
//=======================================================================================


//=======================================================================================
vsodium_ed25519::x25519_secret_key::x25519_secret_key(vsodium_string k)
    :_vsodium_key_(k)
{}
//=======================================================================================
vsodium_ed25519::x25519_public_key vsodium_ed25519::x25519_secret_key::x_public() const
{
    if (!valid()) throw std::runtime_error(__func__);

    vsodium_string pk(crypto_scalarmult_curve25519_BYTES);
    VSODIUM_CALL << crypto_scalarmult_curve25519_base( pk.data(), key().data() );

    return x25519_public_key(pk);
}
//=======================================================================================
bool vsodium_ed25519::x25519_secret_key::valid() const
{
    return key().size() == crypto_scalarmult_curve25519_BYTES;
}
//=======================================================================================
vsodium_string
vsodium_ed25519::x25519_secret_key::scalarmult( const x25519_public_key & other )
{
    if (!valid()) throw std::runtime_error(__func__);
    if (!other.valid()) throw std::runtime_error(__func__);

    vsodium_string res( crypto_scalarmult_curve25519_BYTES );
    VSODIUM_CALL << crypto_scalarmult_curve25519( res.data(),
                                                  key().data(),
                                                  other.key().data() );

    return res;
}
//=======================================================================================
vsodium_ed25519::x25519_public_key::x25519_public_key(vsodium_string k)
    :_vsodium_key_(k)
{}
//=======================================================================================
bool vsodium_ed25519::x25519_public_key::valid() const
{
    return key().size() == crypto_scalarmult_curve25519_BYTES;
}
//=======================================================================================


//=======================================================================================
bool vsodium_ed25519::full::random_load_save(std::string fname)
{
    bool ok = ed_secret.load(fname);
    if ( !ok )
    {
        ed_secret.random();
        ok = ed_secret.save(fname);
    }
    fill();

    return ok;
}
//=======================================================================================
void vsodium_ed25519::full::random()
{
    ed_secret.random();
    fill();
}
//=======================================================================================
void vsodium_ed25519::full::fill()
{
    ed_public = ed_secret.ed_public();

    x_secret = ed_secret.x_secret();
    x_public = ed_public.x_public();
}
//=======================================================================================
static auto _additional_phrases_ = {
    "Lorem ipsum 42",
    "Dog's health",
    "Ophiophagus hannah",
    "Pokerface",
    "Factorio"
};
static vsodium_string additional_shared_hash( const vsodium_string& prev_step )
{
    vsodium_string res = prev_step;
    for ( auto & phrase: _additional_phrases_ )
    {
        res = (res + vsodium_string{phrase}).sha512();
    }
    return res;
}
//=======================================================================================
vsodium_string vsodium_ed25519::full::shared_client(const x25519_public_key &server_key)
{
    if ( !server_key.valid() ) return {};
    if ( !x_secret.valid()   ) return {};
    if ( !x_public.valid()   ) return {};

    auto mult = x_secret.scalarmult( server_key );
    auto h = mult + x_public.key() + server_key.key();

    return additional_shared_hash( h.sha512() );
}
//=======================================================================================
vsodium_string vsodium_ed25519::full::shared_server(const x25519_public_key &client_key)
{
    if ( !client_key.valid() ) return {};
    if ( !x_secret.valid()   ) return {};
    if ( !x_public.valid()   ) return {};

    auto mult = x_secret.scalarmult( client_key );
    auto h = mult + client_key.key() + x_public.key();

    return additional_shared_hash( h.sha512() );
}
//=======================================================================================


//=======================================================================================
void vsodium_ed25519::x25519_secret_key::test()
{
    using std::cout; using std::endl;

    vsodium_ed25519 ed1, ed2;
    ed1.random();
    ed2.random();

    auto p1 = ed1.ed_public();
    auto p2 = ed2.ed_public();

    auto xs1 = ed1.x_secret();
    auto xs2 = ed2.x_secret();

    auto xp1 = xs1.x_public();
    auto xp2 = p2.x_public();  // from different way

    // p1 && p2 have published as non-secret data.
    auto sh1 = xs1.scalarmult(xp2);
    auto sh2 = xs2.scalarmult(xp1);

    if (sh1 != sh2) throw std::runtime_error(__func__);
    //cout << sh1.to_hex() << " == " << sh2.to_hex() << endl;
}
//=======================================================================================
void vsodium_ed25519::test()
{
    vsodium_ed25519::x25519_secret_key::test();
    using std::cout; using std::endl;
    vsodium_ed25519 ed;
    ed.random();
    auto cip = ed.ed_public();

    for (int i = 106; i < 109; ++i)
    {
        vsodium_string text;
        text.random(i);
        auto sm = ed.sign(text);
        auto res = cip.verify(sm);
        if (text != res) throw std::runtime_error(__func__);
        auto ptr = sm.data() + sm.size() - 1;
        ++(*ptr);
        bool ok = true;
        res = cip.verify(sm, &ok);
        if (ok) throw std::runtime_error(__func__);
    }

    auto sec = ed.x_secret();
    auto xp1 = sec.x_public();
    auto xp2 = cip.x_public();
    if (xp1.key() != xp2.key()) throw std::runtime_error(__func__);
}
//=======================================================================================
