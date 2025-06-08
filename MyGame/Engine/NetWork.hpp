#pragma once
#include <enet/enet.h>
#include <string>
#include <functional>  
#include <cstddef>  

class NetWork {
public:
    enum Role { None, Host, Client };

    static NetWork& Instance();

    bool  Init();
    void  Deinit();

    bool  StartHost(enet_uint16 port, size_t maxPeers = 1);
    bool  ConnectToHost(const std::string& host, enet_uint16 port);

    void  Service(float timeoutMs = 0);

    bool  Send(const void* data, size_t len, enet_uint8 channel = 0,
               enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);

    void  SetReceiveCallback(const std::function<void(const ENetEvent&)>& cb);

    void  Shutdown();

    Role  role() const { return _role; }
    bool  isConnected() const;
    ENetHost* GetHost() const { return _host; }

private:
    NetWork();
    ~NetWork();

    ENetHost*   _host      = nullptr;
    ENetPeer*   _peer      = nullptr;
    Role        _role      = None;
    std::function<void(const ENetEvent&)> _onReceive;
};
