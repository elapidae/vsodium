#ifndef VSODIUM_CURVE25519_H
#define VSODIUM_CURVE25519_H

#include "vsodium_string.h"

//=======================================================================================
class _vsodium_key_
{
public:
    _vsodium_key_( const vsodium_string& k = {} );

    const vsodium_string& key() const;

protected:
    vsodium_string _key;
};
//=======================================================================================
class vsodium_ed25519 : public _vsodium_key_
{
public:
    class full;

    class public_key;
    class x25519_public_key;
    class x25519_secret_key;

    void random();
    bool load(std::string fname);
    bool save(std::string fname);

    vsodium_string sign( const vsodium_string& data ) const;

    bool valid() const;

    public_key ed_public() const;
    x25519_secret_key x_secret() const;

    static void test();
};
//=======================================================================================
class vsodium_ed25519::public_key : public _vsodium_key_
{
public:
    explicit public_key( const vsodium_string& pk = {} );

    bool valid() const;

    vsodium_string verify( const vsodium_string& sign, bool *ok = nullptr );

    x25519_public_key x_public() const;
};
//=======================================================================================
class vsodium_ed25519::x25519_public_key : public _vsodium_key_
{
public:
    explicit x25519_public_key(vsodium_string k = {});

    bool valid() const;
};
//=======================================================================================
class vsodium_ed25519::x25519_secret_key : public _vsodium_key_
{
public:
    explicit x25519_secret_key( const vsodium_string& k = {});

    x25519_public_key x_public() const;

    bool valid() const;

    vsodium_string scalarmult( const x25519_public_key& other );

    static void test();
};
//=======================================================================================
class vsodium_ed25519::full
{
public:
    vsodium_ed25519 ed_secret;
    vsodium_ed25519::public_key ed_public;
    vsodium_ed25519::x25519_secret_key x_secret;
    vsodium_ed25519::x25519_public_key x_public;

    bool random_load_save(std::string fname);

    void random();

    void fill();

    vsodium_string shared_client( const x25519_public_key& server_key );
    vsodium_string shared_server( const x25519_public_key& client_key );
};
//=======================================================================================


#endif // VSODIUM_CURVE25519_H
