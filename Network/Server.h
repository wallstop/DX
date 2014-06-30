
#pragma once

namespace DX {
namespace Network {

    struct Port;

    class Server
    {

        virtual ~Server();

        bool listenOn(const Port&);





    };



}
}