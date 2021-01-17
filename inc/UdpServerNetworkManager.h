/*
*   UdpServerNetworkManager manages server-side UDP socket communication
*
*   Author: Fu Qiao 
*   Email:  fqiao@protonmail.com
*
*/

#ifndef __UDP_SERVER_NETWORK_MANAGER_H__
#define __UDP_SERVER_NETWORK_MANAGER_H__

#include "NetworkServiceManager.h"
#include "Common.hpp"
#include <atomic>

namespace sobertalk {

using namespace network;

class UdpServerNetworkManager : public NetworkServiceManager {

public:
 UdpServerNetworkManager(uint16_t port, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out);

 ~UdpServerNetworkManager();

 void HandleRequestIn() override;

 void HandleRequestOut() override;

 void Start() override;

private:
 void Init() override;

 UdpServerNetworkManager(const UdpServerNetworkManager& other);
 UdpServerNetworkManager& operator=(const UdpServerNetworkManager& other);

 std::unique_ptr<UdpSocket> _listener {nullptr};
 uint16_t _port;

};
}
#endif