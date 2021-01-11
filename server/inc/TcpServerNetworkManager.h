/*
*   TcpServerNetworkManager manages server-side TCP socket communication
*
*   Author: Fu Qiao 
*   Email:  qiaofuphysics@gmail.com
*
*/


#ifndef __TCP_SERVER_NETWORK_MANAGER__
#define __TCP_SERVER_NETWORK_MANAGER__

#include "NetworkServiceManager.h"

namespace sobertalk {

class TcpServerNetworkManager : public NetworkServiceManager {

public:
  TcpServerNetworkManager(std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out);

  ~TcpServerNetworkManager();

  void SendMessage(const SocketMessage& msg) override;

  void RecvMessage() override;

private:
  void Init() override;

  TcpServerNetworkManager(const TcpServerNetworkManager& other);

  TcpServerNetworkManager& operator=(const TcpServerNetworkManager& other);

  std::unique_ptr<TcpSocket> _listener {nullptr};

};
}

#endif