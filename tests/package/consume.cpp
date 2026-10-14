/*
 * Consumer smoke test: use the installed zcore package from C++.
 */

#include <iostream>

#include <zcore/zcore.hpp>

int main()
{
    zcore::Buffer buf;
    if (zcore::Error err = zcore::Buffer::create(32, &buf); !zcore::ok(err)) {
        std::cerr << "create failed: " << err << '\n';
        return 1;
    }
    if (zcore::Error err = buf.append("installed", 9); !zcore::ok(err)) {
        std::cerr << "append failed: " << err << '\n';
        return 1;
    }
    if (buf.size() != 9U) {
        std::cerr << "unexpected size: " << buf.size() << '\n';
        return 1;
    }
    if (!zcore::version_is_compatible()) {
        std::cerr << "header/library version mismatch\n";
        return 1;
    }

    std::cout << "consume_cxx: linked zcore " << zcore::version_string() << '\n';
    return 0;
}
