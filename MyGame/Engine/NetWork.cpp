#include "NetWork.hpp"
#include <iostream>

NetWork& NetWork::Instance() {
    static NetWork inst;
    return inst;
}

NetWork::NetWork() {}
NetWork::~NetWork() { Deinit(); }

bool NetWork::Init() {
    if (enet_initialize() != 0) return false;
    atexit(enet_deinitialize);
    return true;
}

void NetWork::Deinit() {
    if (_host) {
        enet_host_destroy(_host);
        _host = nullptr;
    }
    _peer = nullptr;
    _role = None;
}

bool NetWork::StartHost(enet_uint16 port, size_t maxPeers) {
    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = port;
    _host = enet_host_create(&addr, maxPeers, 2, 0, 0);
    if (!_host) return false;
    _role = Host;
    return true;
}

bool NetWork::ConnectToHost(const std::string& host, enet_uint16 port) {
    _host = enet_host_create(nullptr /* no incoming */, 1, 2, 0, 0);
    if (!_host) return false;
    ENetAddress addr;
    enet_address_set_host(&addr, host.c_str());
    addr.port = port;
    _peer = enet_host_connect(_host, &addr, 2, 0);
    if (!_peer) return false;
    _role = Client;
    return true;
}

void NetWork::Service(float timeoutMs) {
    if (!_host) return;
    ENetEvent event;
    while (enet_host_service(_host, &event, static_cast<enet_uint32>(timeoutMs)) > 0) {
        if (_onReceive && event.type == ENET_EVENT_TYPE_RECEIVE) {
            _onReceive(event);
        }
    }
}

bool NetWork::Send(const void* data, size_t len, enet_uint8 channel, enet_uint32 flags) {
    if (!_peer && _role == Host && !_host->peers) return false;
    ENetPacket* pkt = enet_packet_create(data, len, flags);
    enet_peer_send(_peer ? _peer : &_host->peers[0], channel, pkt);
    enet_host_flush(_host);
    return true;
}

void NetWork::SetReceiveCallback(const std::function<void(const ENetEvent&)>& cb) {
    _onReceive = cb;
}

bool NetWork::isConnected() const {
    if (_role == Host) {
        if (!_host) return false;
        for (size_t i = 0; i < _host->peerCount; ++i) {
            if (_host->peers[i].state == ENET_PEER_STATE_CONNECTED)
                return true;
        }
        return false;
    }
    if (_role == Client) {
        return _peer && _peer->state == ENET_PEER_STATE_CONNECTED;
    }
    return false;
}

void NetWork::Shutdown() {
    Deinit();
}
