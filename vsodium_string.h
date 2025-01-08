#ifndef VSODIUM_STRING_H
#define VSODIUM_STRING_H

#include <ostream>
#include <memory>
#include <string_view>
#include <limits>

//=======================================================================================
class vsodium_string final
{
public:
    vsodium_string();
    ~vsodium_string();

    //vsodium_string(const std::string& data);
    explicit vsodium_string(size_t size);
    //explicit vsodium_string(const char *);
    explicit vsodium_string(std::string_view view);

    vsodium_string(vsodium_string&&) = default;
    vsodium_string(const vsodium_string&) = default;
    vsodium_string& operator = (vsodium_string&&) = default;
    vsodium_string& operator = (const vsodium_string&) = default;

    vsodium_string& operator += (const vsodium_string& rhs);
    vsodium_string operator + (const vsodium_string& rhs) const;

    bool operator == (const vsodium_string& rhs) const;
    bool operator != (const vsodium_string& rhs) const;

    std::string str() const;
    std::string_view view() const;

    vsodium_string to_hex() const;
    vsodium_string from_hex() const;

    vsodium_string to_base64() const;
    vsodium_string from_base64() const;

    vsodium_string sha256() const;
    vsodium_string sha512() const;

    bool empty() const;
    size_t size() const;

    const unsigned char * data() const;
    unsigned char * data();

    const unsigned char * const_data() const;

    const char * sdata() const;
    char * sdata();

    void resize(size_t size);

    // correct size without reallocation.
    void decrement_size_to(size_t size);

    unsigned char at(size_t pos) const;

    vsodium_string right(size_t size) const;
    vsodium_string mid(size_t pos, size_t len) const;

    void random(size_t size);
    static uint32_t rand32(uint32_t upper_bound = std::numeric_limits<uint32_t>::max());

    static void test();

private:
    class pimpl; std::shared_ptr<pimpl> p;
    void assert_p() const;
};
//=======================================================================================
std::ostream& operator << (std::ostream& os, const vsodium_string& str);
//=======================================================================================


#endif // VSODIUM_STRING_H
