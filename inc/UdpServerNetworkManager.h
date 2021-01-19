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
#include <atomic>

namespace sobertalk {

class UdpServerNetworkManager : public NetworkServiceManager {

using UdpSocket = network::UdpSocket;
using NetworkRequest = common::NetworkRequest;

public:
 UdpServerNetworkManager(uint16_t port, std::shared_ptr<SocketMessageQueue> queue_In, std::shared_ptr<SocketMessageQueue> queue_Out);

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