#ifndef VSODIUM_AES256GCM_H
#define VSODIUM_AES256GCM_H

#include "vsodium_string.h"

class vsodium_aes256gcm
{
public:
    explicit vsodium_aes256gcm( const vsodium_string& init = {} );
};

#endif // VSODIUM_AES256GCM_H
