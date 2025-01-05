#include <iostream>

#include "vsodium_string.h"
#include "vsodium_curve25519.h"
#include "vlog.h"
#include "vsodium_xchacha20poly1305_ietf.h"
using namespace std;

//=======================================================================================
int main()
{
    vsodium_string::test();
    vsodium_ed25519::test();
    vdeb << "======================================";

    auto fname = ".id_ed25519";
    vsodium_ed25519::full ed25519;
    vdeb << "loaded/saved:" << ed25519.random_load_save(fname);

    vsodium_ed25519::full serv;
    serv.random();
    auto sec1 = ed25519.shared_client(serv.x_public);
    vdeb << sec1.to_hex();

    auto sec2 = serv.shared_server(ed25519.x_public);
    vdeb << sec2.to_hex();

    vsodium_xchacha20poly1305_ietf aes(sec2);

    auto cip = aes.encrypt( vsodium_string("test") );
    vdeb << cip << cip.to_hex();
    vdeb << aes.decrypt(cip);

    return 0;
}
//=======================================================================================
