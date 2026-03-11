#include "vsodium_emodji.h"

#include <vector>
#include <set>
#include <unistd.h>

#include "vlog.h"
#include "vcat.h"
#include"vbyte_buffer.h"

//=======================================================================================
static auto usable_diapazone = R"(
2600..26FF
2700..27BF
1F300..1F5FF
1F600..1F64F
1F680..1F6FF
1F700..1F77F
1F780..1F7FF
1F900..1F9FF
1FA00..1FA6F
1FA70..1FAFF
1FB00..1FBFF)";
//=======================================================================================
static auto exclude_diapazone = R"(
1F3FB..1F3FF   (skin tone modifiers)
FE0E..FE0F     (variation selectors)
1F1E6..1F1FF   (regional indicators / флаги)
200D           (zero width joiner)
)";
//=======================================================================================
struct diapazone
{
    using vector = std::vector<diapazone>;
    using set = std::set<uint32_t>;

    uint32_t from;
    uint32_t to;

    //-----------------------------------------------------------------------------------
    set to_set() const
    {
        set res;
        for ( auto i = from; i <= to; ++i )
        {
            res.insert(i);
        }
        return res;
    }
    //-----------------------------------------------------------------------------------
};
//=======================================================================================
std::ostream& operator << ( std::ostream& s, const diapazone& d )
{
    s << std::hex << "diap[" << d.from << ":" << d.to << "]";
    return s;
}
//=======================================================================================
static diapazone::vector parse_diapazone( const std::string& str )
{
    diapazone::vector res;
    auto lines = vbyte_buffer(str).split('\n');
    for ( auto& line : lines )
    {
        if ( line.empty() ) continue;
        auto diapazone = line.split_by_spaces().at(0);

        auto range = diapazone.split("..");
        if ( range.size() == 1 )
        {
            range.push_back( range.at(0) );
        }
        if ( range.size() != 2 )
        {
            throw verror << "range:" << range;
        }

        auto from = range.at(0).to_hex_u32();
        auto to = range.at(1).to_hex_u32();

        uint64_t test_from = std::stoull(range.at(0).data(), nullptr, 16);
        uint64_t test_to = std::stoull(range.at(1).data(), nullptr, 16);

        if ( from != test_from || to != test_to )
        {
            throw verror << "from:" << from << " != " << test_from
                         << " or to:" << to << " != " << test_to;
        }

        res.push_back( {from, to} );
    }
    return res;
}
//=======================================================================================
struct emodji
{
    //-----------------------------------------------------------------------------------
    using vector32 = std::vector<uint32_t>;
    //-----------------------------------------------------------------------------------
    explicit emodji(vector32 v)
        : code_points( std::move(v) )
    {}
    //-----------------------------------------------------------------------------------
    static std::string codepoint_to_utf8(uint32_t cp)
    {
        std::string out;

        if (cp <= 0x7F)
        {
            out.push_back(cp);
        }
        else if (cp <= 0x7FF)
        {
            out.push_back(0xC0 | (cp >> 6));
            out.push_back(0x80 | (cp & 0x3F));
        }
        else if (cp <= 0xFFFF)
        {
            out.push_back(0xE0 | (cp >> 12));
            out.push_back(0x80 | ((cp >> 6) & 0x3F));
            out.push_back(0x80 | (cp & 0x3F));
        }
        else if (cp <= 0x10FFFF)
        {
            out.push_back(0xF0 | (cp >> 18));
            out.push_back(0x80 | ((cp >> 12) & 0x3F));
            out.push_back(0x80 | ((cp >> 6) & 0x3F));
            out.push_back(0x80 | (cp & 0x3F));
        } else {
            throw verror << "Invalid Unicode code point: " << cp;
        }

        return out;
    }
    //-----------------------------------------------------------------------------------
    uint32_t at_cp( int index ) const
    {
        if ( code_points.empty() )
            throw std::runtime_error("No code points in emodji");

        if ( index >= code_points.size() )
            throw std::runtime_error("index in emodji");

        return code_points.at( index );
    }
    //-----------------------------------------------------------------------------------
    std::string at( int index ) const
    {
        if ( code_points.empty() )
            throw std::runtime_error("No code points in emodji");

        if ( index >= code_points.size() )
            throw std::runtime_error("index in emodji");

        auto cp = code_points.at( index );
        return codepoint_to_utf8( cp );
    }
    //-----------------------------------------------------------------------------------
    size_t size() const
    {
        return code_points.size();
    }
    //-----------------------------------------------------------------------------------
    std::string get_rem( uint16_t word ) const
    {
        if ( code_points.empty() )
        {
            throw std::runtime_error("No code points in emodji");
        }
        auto index = word % code_points.size();
        auto cp = code_points.at( index );
        return codepoint_to_utf8( cp );
    }
    //-----------------------------------------------------------------------------------
    const vector32& code_points_list() const
    {
        return code_points;
    }
    //-----------------------------------------------------------------------------------
private:
    vector32 code_points;
}; // emodji
//=======================================================================================


//=======================================================================================
struct  emodji_builder
{
    void include_diapazone(diapazone::vector diapazones)
    {
        for ( auto& diap : diapazones )
        {
            auto set = diap.to_set();
            for ( auto val: set )
            {
                set_code_points.insert( val );
            }
        }
    }
    void exclude_diapazone(diapazone::vector diapazones)
    {
        for ( auto& diap : diapazones )
        {
            auto set = diap.to_set();
            for ( auto val: set )
            {
                set_code_points.erase( val );
            }
        }
    }

    emodji build() const
    {
        emodji::vector32 vec( set_code_points.begin(), set_code_points.end() );
        std::sort( vec.begin(), vec.end() );
        return emodji( vec );
    }

private:
    std::set<uint32_t> set_code_points;
}; // emodji_builder
//---------------------------------------------------------------------------------------
static const emodji& default_emodji()
{
    static const emodji *res{ nullptr };
    if ( res )
    {
        return *res;
    }

    auto includes = parse_diapazone(usable_diapazone);
    auto excludes = parse_diapazone(exclude_diapazone);

    vdeb << includes;
    vdeb << excludes;

    emodji_builder builder;
    builder.include_diapazone(includes);
    builder.exclude_diapazone(excludes);
    auto em = builder.build();

    //vdeb << em.code_points_list();
    vdeb << em.code_points_list().size();

    res = new emodji( em );
    return *res;
}
//=======================================================================================

//=======================================================================================
std::string vsodium_emodji::get_emodji_for(uint16_t word)
{
    return default_emodji().get_rem( word );
}
//=======================================================================================
void vsodium_emodji::test()
{
    auto &em = default_emodji();

    static auto line_len = 32;
    for ( auto i = 0; i < em.size(); i+= line_len )
    {
        auto pos0 = em.at_cp(i);
        uint32_t pos1 = 0;

        std::string em_buf;
        for ( auto row = 0; row < line_len; ++row )
        {
            auto idx = i + row;
            if ( idx >= em.size() )
                break;

            pos1 = em.at_cp( idx );
            em_buf += em.at( idx );
        }

        vcat line;
        line.hex()(pos0)(" -> ")(em_buf)(" <- ").hex()(pos1);
        vdeb << line.str();
    }
}
//=======================================================================================
