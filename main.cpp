#include <iostream>

#include "vsodium_string.h"
#include "vsodium_curve25519.h"
#include "vlog.h"
#include "vsodium_xchacha20poly1305_ietf.h"
#include "vsodium_key_exchange.h"
using namespace std;

//=======================================================================================
int main()
{
    vsodium_string::test();
    vsodium_ed25519::test();
    vsodium_xchacha20poly1305_ietf::test();
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

    for (int i = 0; i < 100; ++i)
    {
        vsodium_string msg;
        msg.random(i);
        auto cip = aes.encrypt( msg );
        //vdeb << msg.size() << cip.size();
        auto dec = aes.decrypt(cip);
        if (dec != msg) throw verror;
    }

    for (int i = 0; i < 100; ++i)
    {
        //vdeb << vsodium_string::rand32(i) << '\r';
    }

    bool ok;
    vsodium_key_exchange kx1, kx2;
    auto rt1 = kx1.shared_client( kx2.pub_key(), &ok );
    auto rt2 = kx2.shared_server( kx1.pub_key(), &ok );

    vdeb << rt1.rx.to_hex() << rt2.tx.to_hex();
    vdeb << rt2.rx.to_hex() << rt1.tx.to_hex();

    return 0;
}
//=======================================================================================
