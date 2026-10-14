/*
 * zcorelib example: the C++ wrapper's house style.
 *
 * The wrapper buys exactly one thing over the C API -- it destroys the buffer
 * for you. Errors are still zc_error_t return values, because the library
 * does not throw. Compare this with examples/buffer_c.c: same calls, no
 * cleanup paths.
 */

#include <iostream>
#include <string_view>

#include <zcore/zcore.hpp>

namespace {

zcore::Error AppendLine(zcore::Buffer &buf, std::string_view line)
{
    return buf.append(line.data(), line.size());
}

} // namespace

int main()
{
    static constexpr std::string_view kLines[] = {"alpha\n", "beta\n", "gamma\n"};

    zcore::Buffer buf;
    if (const zcore::Error err = zcore::Buffer::create(64, &buf); !zcore::ok(err)) {
        std::cerr << "could not create buffer: " << err << '\n';
        return 1;
    }

    for (const std::string_view line : kLines) {
        if (const zcore::Error err = AppendLine(buf, line); !zcore::ok(err)) {
            std::cerr << "append failed: " << err << '\n';
            return 1; // no cleanup needed: Buffer's destructor handles it
        }
    }

    std::cout << "zcore " << zcore::version_string() << '\n'
              << "size=" << buf.size() << " capacity=" << buf.capacity() << '\n'
              << "contents:\n"
              << std::string_view(static_cast<const char *>(buf.data()), buf.size());

    return 0;
}
