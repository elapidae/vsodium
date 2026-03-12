#include "vsodium_emodji.h"

#include <vector>
#include <set>
#include <unistd.h>

#include "vlog.h"
#include "vcat.h"
#include"vbyte_buffer.h"
#include"vbyte_buffer_view.h"

#include "vsodium_string.h"

//=======================================================================================
static auto usable_diapazone = R"(
# 2600..26FF
# 2700..27BF
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
// Deleted all strange lines;
static auto log_of_work_emodji_v1 = R"(
[vsodium_emodji.cpp:281] debug:	0x1f340 -> 🍀🍁🍂🍃🍄🍅🍆🍇🍈🍉🍊🍋🍌🍍🍎🍏🍐🍑🍒🍓🍔🍕🍖🍗🍘🍙🍚🍛🍜🍝🍞🍟 <- 0x1f35f
[vsodium_emodji.cpp:281] debug:	0x1f360 -> 🍠🍡🍢🍣🍤🍥🍦🍧🍨🍩🍪🍫🍬🍭🍮🍯🍰🍱🍲🍳🍴🍵🍶🍷🍸🍹🍺🍻🍼🍽🍾🍿 <- 0x1f37f
[vsodium_emodji.cpp:281] debug:	0x1f3a0 -> 🎠🎡🎢🎣🎤🎥🎦🎧🎨🎩🎪🎫🎬🎭🎮🎯🎰🎱🎲🎳🎴🎵🎶🎷🎸🎹🎺🎻🎼🎽🎾🎿 <- 0x1f3bf
[vsodium_emodji.cpp:281] debug:	0x1f3e0 -> 🏠🏡🏢🏣🏤🏥🏦🏧🏨🏩🏪🏫🏬🏭🏮🏯🏰🏱🏲🏳🏴🏵🏶🏷🏸🏹🏺🐀🐁🐂🐃🐄 <- 0x1f404
[vsodium_emodji.cpp:281] debug:	0x1f405 -> 🐅🐆🐇🐈🐉🐊🐋🐌🐍🐎🐏🐐🐑🐒🐓🐔🐕🐖🐗🐘🐙🐚🐛🐜🐝🐞🐟🐠🐡🐢🐣🐤 <- 0x1f424
[vsodium_emodji.cpp:281] debug:	0x1f425 -> 🐥🐦🐧🐨🐩🐪🐫🐬🐭🐮🐯🐰🐱🐲🐳🐴🐵🐶🐷🐸🐹🐺🐻🐼🐽🐾🐿👀👁👂👃👄 <- 0x1f444
[vsodium_emodji.cpp:281] debug:	0x1f445 -> 👅👆👇👈👉👊👋👌👍👎👏👐👑👒👓👔👕👖👗👘👙👚👛👜👝👞👟👠👡👢👣👤 <- 0x1f464
[vsodium_emodji.cpp:281] debug:	0x1f465 -> 👥👦👧👨👩👪👫👬👭👮👯👰👱👲👳👴👵👶👷👸👹👺👻👼👽👾👿💀💁💂💃💄 <- 0x1f484
[vsodium_emodji.cpp:281] debug:	0x1f485 -> 💅💆💇💈💉💊💋💌💍💎💏💐💑💒💓💔💕💖💗💘💙💚💛💜💝💞💟💠💡💢💣💤 <- 0x1f4a4
[vsodium_emodji.cpp:281] debug:	0x1f4c5 -> 📅📆📇📈📉📊📋📌📍📎📏📐📑📒📓📔📕📖📗📘📙📚📛📜📝📞📟📠📡📢📣📤 <- 0x1f4e4
[vsodium_emodji.cpp:281] debug:	0x1f4e5 -> 📥📦📧📨📩📪📫📬📭📮📯📰📱📲📳📴📵📶📷📸📹📺📻📼📽📾📿🔀🔁🔂🔃🔄 <- 0x1f504
[vsodium_emodji.cpp:281] debug:	0x1f645 -> 🙅🙆🙇🙈🙉🙊🙋🙌🙍🙎🙏🚀🚁🚂🚃🚄🚅🚆🚇🚈🚉🚊🚋🚌🚍🚎🚏🚐🚑🚒🚓🚔 <- 0x1f694
[vsodium_emodji.cpp:281] debug:	0x1f695 -> 🚕🚖🚗🚘🚙🚚🚛🚜🚝🚞🚟🚠🚡🚢🚣🚤🚥🚦🚧🚨🚩🚪🚫🚬🚭🚮🚯🚰🚱🚲🚳🚴 <- 0x1f6b4
[vsodium_emodji.cpp:281] debug:	0x1f915 -> 🤕🤖🤗🤘🤙🤚🤛🤜🤝🤞🤟🤠🤡🤢🤣🤤🤥🤦🤧🤨🤩🤪🤫🤬🤭🤮🤯🤰🤱🤲🤳🤴 <- 0x1f934
[vsodium_emodji.cpp:281] debug:	0x1f935 -> 🤵🤶🤷🤸🤹🤺🤻🤼🤽🤾🤿🥀🥁🥂🥃🥄🥅🥆🥇🥈🥉🥊🥋🥌🥍🥎🥏🥐🥑🥒🥓🥔 <- 0x1f954
[vsodium_emodji.cpp:281] debug:	0x1f955 -> 🥕🥖🥗🥘🥙🥚🥛🥜🥝🥞🥟🥠🥡🥢🥣🥤🥥🥦🥧🥨🥩🥪🥫🥬🥭🥮🥯🥰🥱🥲🥳🥴 <- 0x1f974
[vsodium_emodji.cpp:281] debug:	0x1f975 -> 🥵🥶🥷🥸🥹🥺🥻🥼🥽🥾🥿🦀🦁🦂🦃🦄🦅🦆🦇🦈🦉🦊🦋🦌🦍🦎🦏🦐🦑🦒🦓🦔 <- 0x1f994
[vsodium_emodji.cpp:281] debug:	0x1f995 -> 🦕🦖🦗🦘🦙🦚🦛🦜🦝🦞🦟🦠🦡🦢🦣🦤🦥🦦🦧🦨🦩🦪🦫🦬🦭🦮🦯🦰🦱🦲🦳🦴 <- 0x1f9b4
[vsodium_emodji.cpp:281] debug:	0x1f9b5 -> 🦵🦶🦷🦸🦹🦺🦻🦼🦽🦾🦿🧀🧁🧂🧃🧄🧅🧆🧇🧈🧉🧊🧋🧌🧍🧎🧏🧐🧑🧒🧓🧔 <- 0x1f9d4
[vsodium_emodji.cpp:281] debug:	0x1f9d5 -> 🧕🧖🧗🧘🧙🧚🧛🧜🧝🧞🧟🧠🧡🧢🧣🧤🧥🧦🧧🧨🧩🧪🧫🧬🧭🧮🧯🧰🧱🧲🧳🧴 <- 0x1f9f4
)";
//=======================================================================================


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
        line.trim_spaces();
        if ( line.empty() ) continue;
        if ( line.starts_with("#") ) continue;

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
    static uint32_t utf8_extract_next_codepoint( std::string *src )
    {
        //-------------------------------------------------------------------------
        auto take_next_sym = [src]()
        {
            auto ch = src->at(0);
            src->erase(0, 1);

            if ( (ch & 0xC0) != 0x80 )
                throw verror << "Invalid UTF-8 next sequence";

            return uint32_t(ch & 0x3F);
        };
        //-------------------------------------------------------------------------
        uint32_t res = 0;
        auto shift_6_and_take_next_sym = [&res, &take_next_sym]()
        {
            res <<= 6;
            res |= take_next_sym();
        };

        unsigned char c0 = src->at(0);
        //-------------------------------------------------------------------------
        if ( (c0 & 0x80) == 0 )
        {
            res = c0;
            src->erase(0, 1);
            return res;
        }
        //-------------------------------------------------------------------------
        if ( (c0 & 0xE0) == 0xC0 )
        {
            res = (c0 & 0x1F);
            src->erase(0, 1);

            shift_6_and_take_next_sym();

            return res;
        }
        //-------------------------------------------------------------------------
        if ( (c0 & 0xF0) == 0xE0 )
        {
            res = (c0 & 0x0F);
            src->erase(0, 1);

            shift_6_and_take_next_sym();
            shift_6_and_take_next_sym();

            return res;
        }
        //-------------------------------------------------------------------------
        if ( (c0 & 0xF8) == 0xF0 )
        {
            res = (c0 & 0x07);
            src->erase(0, 1);

            shift_6_and_take_next_sym();
            shift_6_and_take_next_sym();
            shift_6_and_take_next_sym();

            return res;
        }
        //-------------------------------------------------------------------------
        throw verror << "Invalid UTF-8 sequence";
        //-------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
//=======================================================================================
struct Parse_log_of_work_emodji_v1
{
    std::vector<uint32_t> points;

    static const Parse_log_of_work_emodji_v1& instance()
    {
        static const Parse_log_of_work_emodji_v1 res;
        return res;
    }

    void print() const
    {
        vcat res;
        for ( auto cp : points )
        {
            res( emodji::codepoint_to_utf8(cp) );
        }
        vdeb << res;
    }

private:
    Parse_log_of_work_emodji_v1()
    {
        auto lines = vbyte_buffer(log_of_work_emodji_v1).split('\n');
        for ( auto& line : lines )
        {
            line.trim_spaces();
            if ( line.empty() ) continue;
            if ( line.starts_with("#") ) continue;

            auto parts = line.split("->");
            if ( parts.size() != 2 )
                throw verror << "line st0:" << line;

            parts = parts.at(1).split("<-");
            if ( parts.size() != 2 )
                throw verror << "line st1:" << line;

            auto emodjis_str = parts.at(0).trim_spaces().str();
            while ( !emodjis_str.empty() )
            {
                auto cp = emodji::utf8_extract_next_codepoint( &emodjis_str );
                points.push_back( cp );
            }
        }
    }
};
//=======================================================================================
static const emodji& default_emodji_v1()
{
    static const emodji *res{ nullptr };
    if ( res )
    {
        return *res;
    }


    auto includes = parse_diapazone(log_of_work_emodji_v1);

    vdeb << includes;

    emodji_builder builder;
    builder.include_diapazone(includes);
    auto em = builder.build();

    vdeb << em.code_points_list();
    vdeb << em.code_points_list().size();

    return *res;
}
//=======================================================================================


//=======================================================================================
std::string vsodium_emodji::get_emodji_for(uint16_t word)
{
    return default_emodji().get_rem( word );
}
//=======================================================================================

std::string vsodium_emodji::work_emodji_v1(uint16_t word)
{
    auto &points = Parse_log_of_work_emodji_v1::instance().points;

    if ( points.empty() )
        throw std::runtime_error("No code points in emodji");

    auto point = points.at( word % points.size() );
    return emodji::codepoint_to_utf8( point );
}
//=======================================================================================
uint16_t vsodium_emodji::work_emodji_v1_size()
{
    return Parse_log_of_work_emodji_v1::instance().points.size();
}
//=======================================================================================
std::string vsodium_emodji::make_emodji_for_data( std::string data , uint8_t symbols_em )
{
    std::string res;

    if ( symbols_em <= 0 )
    {
        throw verror << "size must be positive";
    }
    if ( symbols_em >= vsodium_emodji::work_emodji_v1_size() )
    {
        throw verror << "size must be less than "
                     << vsodium_emodji::work_emodji_v1_size();
    }

    int added = 0;
    while ( added < symbols_em )
    {
        auto mishumi = vsodium_string(data).mishumi_sha512_hash().str();
        vbyte_buffer bb( mishumi );
        auto view = bb.view();
        auto sz = view.remained();
        auto omit = view.show_u8() % sz;
        if ( omit >= sz - 3 )
            omit = sz - 4;
        view.omit( omit );

        auto cp_hash = view.show_u16_LE();

        auto cur_em = vsodium_emodji::work_emodji_v1( cp_hash );
        if ( res.find(cur_em) != std::string::npos )
        {
            continue;
        }
        res += cur_em;
        ++added;
        data = mishumi;
    }

    return res;
}
//=======================================================================================
void vsodium_emodji::test()
{
    Parse_log_of_work_emodji_v1::instance().print();

    vdeb << vsodium_emodji::make_emodji_for_data("Hello, world!");
    vdeb << vsodium_emodji::make_emodji_for_data("Hello, world");
    vdeb << vsodium_emodji::make_emodji_for_data("Hello, worl");
    vdeb << vsodium_emodji::make_emodji_for_data("Hello, wor");
    vdeb << vsodium_emodji::make_emodji_for_data("Hello, wo");
    return;


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
