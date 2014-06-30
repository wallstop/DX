
#include "Address.h"

#include <sstream>

namespace DX {
namespace Network {

Addressv4 Addressv4::fromString(const std::string& input)
{
    std::istringstream buffer(input);
    static Addressv4 ret;
    try
    {
        buffer >> ret.a;
        buffer >> ret.b;
        buffer >> ret.c;
        buffer >> ret.d;
    }
    catch(std::exception& e)
    {
        ; // TODO: Handle errors
    }

    return ret;

}

Addressv4 Addressv4::fromBytes(unsigned char _a, unsigned char _b,
    unsigned char _c, unsigned char _d)
{
    static Addressv4 ret;
    ret.a = _a;
    ret.b = _b;
    ret.c = _c;
    ret.d = _d;

    return ret;
}

Addressv4 Addressv4::fromUnsignedInt(unsigned int address)
{
   static Addressv4 ret;
   const auto&& charPointer = reinterpret_cast<unsigned char* >(&address);
   ret.a = charPointer[0];
   ret.b = charPointer[1];
   ret.c = charPointer[2];
   ret.d = charPointer[3];

   return ret;
}




}
}