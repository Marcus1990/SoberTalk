#include "TcpServerNetworkManager.h"

namespace sobertalk {

TcpServerNetworkManager::TcpServerNetworkManager(std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, 
                                                 std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out)
  : NetworkServiceManager(queue_In, queue_Out) {
}

TcpServerNetworkManager::~TcpServerNetworkManager() {
}





}