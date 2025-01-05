#include "vsodium_string.h"

#include <sodium.h>
#include <stdexcept>

#include "vsodium_init.h"

vsodium_init initer;

//=======================================================================================
class vsodium_string::pimpl final
{
public:
    unsigned char *ptr = nullptr;
    size_t len = 0;

    ~pimpl()
    {
        clear();
    }

    void clear()
    {
        if (!ptr) return;
        sodium_memzero( ptr, len );
        sodium_free( ptr );
        ptr = 0;
        len = 0;
    }

    void alloc(size_t size)
    {
        clear();
        ptr = static_cast<decltype(ptr)>( sodium_malloc(size) );
        if (!ptr) throw std::runtime_error( "sodium_malloc" );
        len = size;
    }

    template<typename Char>
    void set( const Char *_src, size_t size )
    {
        if (len != size)
        {
            clear();
            alloc(size);
        }
        auto vptr = static_cast<const void*>(_src);
        auto src  = static_cast<const unsigned char*>(vptr);
        auto end  = src + size;
        std::copy(src, end, ptr);
    }

    const void* void_ptr() const { return static_cast<const void*> ( ptr ); }
    void* void_ptr()             { return static_cast<void*>       ( ptr ); }

    const char* sdata() const { return static_cast<const char*> ( void_ptr() ); }
    char* sdata()             { return static_cast<char*>       ( void_ptr() ); }
};
//=======================================================================================


//=======================================================================================
vsodium_string::vsodium_string()
    : p( new pimpl )
{}
//=======================================================================================
vsodium_string::~vsodium_string()
{}
//=======================================================================================
//vsodium_string::vsodium_string( const std::string & data )
//    : vsodium_string()
//{
//    p->set( data.data(), data.size() );
//}
//=======================================================================================
vsodium_string::vsodium_string( size_t _size )
    : vsodium_string()
{
    resize( _size );
}
//=======================================================================================
vsodium_string::vsodium_string( std::string_view view )
    : vsodium_string()
{
    p->set( view.data(), view.size() );
}
//=======================================================================================
vsodium_string& vsodium_string::operator += ( const vsodium_string & rhs )
{
    if ( rhs.empty() ) return *this;

    auto res = new pimpl;
    res->alloc( size() + rhs.size() );

    auto begin = const_data();
    auto end = const_data() + size();
    auto dst = res->ptr;
    std::copy( begin, end, dst );

    begin = rhs.data();
    end = rhs.data() + rhs.size();
    dst = res->ptr + p->len;
    std::copy( begin, end, dst );

    p.reset( res );

    return *this;
}
//=======================================================================================
vsodium_string vsodium_string::operator + ( const vsodium_string & rhs ) const
{
    vsodium_string res = *this;
    res += rhs;
    return res;
}
//=======================================================================================
bool vsodium_string::operator ==(const vsodium_string &rhs) const
{
    if ( size() != rhs.size() ) return false;
    auto rcode = sodium_memcmp( data(), rhs.data(), size() );
    return rcode == 0;
}
//=======================================================================================
bool vsodium_string::operator !=(const vsodium_string &rhs) const
{
    return !(*this == rhs);
}
//=======================================================================================
std::string vsodium_string::str() const
{
    assert_p();
    if (!p->ptr) return {};
    return std::string(p->sdata(), p->len);
}
//=======================================================================================
std::string_view vsodium_string::view() const
{
    if (!p) return {};
    return std::string_view(p->sdata(), p->len);
}
//=======================================================================================
bool vsodium_string::empty() const
{
    assert_p();
    if (!p->ptr) return true;
    return p->len == 0;
}
//=======================================================================================
size_t vsodium_string::size() const
{
    assert_p();
    return p->len;
}
//=======================================================================================
const unsigned char *vsodium_string::data() const
{
    assert_p();
    return p->ptr;
}
//=======================================================================================
unsigned char *vsodium_string::data()
{
    assert_p();
    return p->ptr;
}
//=======================================================================================
const unsigned char *vsodium_string::const_data() const
{
    return data();
}
//=======================================================================================
const char *vsodium_string::sdata() const
{
    assert_p();
    return p->sdata();
}
//=======================================================================================
char *vsodium_string::sdata()
{
    assert_p();
    return p->sdata();
}
//=======================================================================================
void vsodium_string::resize( size_t _size )
{
    assert_p();
    if (size() == _size) return;
    p->clear();
    p->alloc(_size);
}
//=======================================================================================
void vsodium_string::decrement_size_to( size_t _size )
{
    if ( _size > size() ) throw std::runtime_error(__func__);
    p->len = _size;
}
//=======================================================================================
unsigned char vsodium_string::at(size_t pos) const
{
    assert_p();
    if (pos >= size()) throw std::out_of_range("vsodium_string::at");
    return p->ptr[pos];
}
//=======================================================================================
vsodium_string vsodium_string::right(size_t _size) const
{
    if (size() <= _size) return *this;

    auto pos = size() - _size;

    auto begin = const_data() + pos;
    auto end = begin + _size;
    vsodium_string res(_size);
    std::copy(begin, end, res.data());
    return res;
}
//=======================================================================================
vsodium_string vsodium_string::mid( size_t pos, size_t len ) const
{
    if ( pos + len > size() ) throw std::runtime_error(__func__);
    vsodium_string res;
    res.p->set( data() + pos, len );
    return res;
}
//=======================================================================================
void vsodium_string::random( size_t _size )
{
    resize(_size);
    randombytes_buf( data(), size() );
}
//=======================================================================================
void vsodium_string::assert_p() const
{
    if (!p) throw std::runtime_error("pimpl assert");
}
//=======================================================================================


//=======================================================================================
vsodium_string vsodium_string::to_hex() const
{
    vsodium_string res;
    res.resize( size()*2 + 1 );
    sodium_bin2hex( res.sdata(), res.size(), data(), size() );

    // TODO: think about refactor it
    // correct ending zero.
    while ( res.size() && res.at(res.size() - 1) == '\0' )
    {
        --res.p->len;
    }

    return res;
}
//=======================================================================================
vsodium_string vsodium_string::from_hex() const
{
    vsodium_string res;
    res.resize( size()/2 );
    size_t reslen;
    auto rcode = sodium_hex2bin( res.data(), res.size(),
                                 sdata(), size(),
                                 nullptr, &reslen,
                                 nullptr);

    if (rcode != 0) return {};

    // TODO: think about refactor it
    res.p->len = reslen;

    return res;
}
//=======================================================================================
auto constexpr base64_variant = sodium_base64_VARIANT_ORIGINAL;
vsodium_string vsodium_string::to_base64() const
{
    auto reslen = sodium_base64_encoded_len( size(), base64_variant );
    vsodium_string res;
    res.resize(reslen);

    sodium_bin2base64( res.sdata(), res.size(),
                       data(), size(),
                       base64_variant );

    // correct ending zero.
    while ( res.size() && res.at(res.size() - 1) == '\0' )
    {
        --res.p->len;
    }

    return res;
}
//=======================================================================================
vsodium_string vsodium_string::from_base64() const
{
    vsodium_string res;
    res.resize( size() / 4 * 3 );

    size_t reslen;
    auto rcode = sodium_base642bin( res.data(), res.size(),
                                    sdata(), size(),
                                    nullptr, &reslen,
                                    nullptr, base64_variant );

    if (rcode != 0) return {};
    res.p->len = reslen;

    return res;
}
//=======================================================================================
vsodium_string vsodium_string::sha256() const
{
    vsodium_string res( crypto_hash_sha256_BYTES );

    auto rcode = crypto_hash_sha256( res.data(), data(), size() );
    if ( rcode != 0 ) return {};

    return res;
}
//=======================================================================================
vsodium_string vsodium_string::sha512() const
{
    vsodium_string res( crypto_hash_sha512_BYTES );

    auto rcode = crypto_hash_sha512( res.data(), data(), size() );
    if ( rcode != 0 ) return {};

    return res;
}
//=======================================================================================


//=======================================================================================
std::ostream& operator << (std::ostream& os, const vsodium_string& str)
{
    os << str.str();
    return os;
}
//=======================================================================================


//=======================================================================================
void vsodium_string::test()
{
    // operator +
    std::string h = "Hello", w = "world";
    auto shw = h + w;
    vsodium_string hh(h);
    vsodium_string ww(w);
    if (shw != (hh+ww).str()) throw std::runtime_error(__func__);
}
//=======================================================================================
