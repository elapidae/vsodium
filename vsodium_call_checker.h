#ifndef VSODIUM_CALL_CHECKER_H
#define VSODIUM_CALL_CHECKER_H

#include <string>

//=======================================================================================
class vsodium_call_checker
{
public:
    void operator << (int rcode) const;
    vsodium_call_checker& operator << (std::string_view info);

private:
    std::string_view info;
};
//=======================================================================================
static vsodium_call_checker vsodium_call;
#define VSODIUM_CALL vsodium_call << __func__
//=======================================================================================

#endif // VSODIUM_CALL_CHECKER_H
