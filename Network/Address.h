
#pragma once

#include <string>

namespace DX {
namespace Network {

    struct Port
    {
        unsigned char port;
    };

    class Addressv4
    {
    public:

        static Addressv4 fromString(const std::string&);
        static Addressv4 fromBytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
        static Addressv4 fromUnsignedInt(unsigned int address);

    private:
        unsigned char a;
        unsigned char b;
        unsigned char c;
        unsigned char d;
    };
    
}
}