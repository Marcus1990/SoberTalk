#include "UdpServerNetworkManager.h"

namespace sobertalk {

UdpServerNetworkManager::UdpServerNetworkManager(uint16_t port, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out)
 : NetworkServiceManager(queue_In, queue_Out), _port(port) {

}

UdpServerNetworkManager::~UdpServerNetworkManager() {
}

void UdpServerNetworkManager::Init() {
 
 _listener = std::make_unique<UdpSocket>(NULL, _port);
 if (!_queue_in) {
   _queue_in = std::make_shared<ConcurrentQueue<SocketMessage>>();
 }

 if (!_queue_out) {
   _queue_out = std::make_shared<ConcurrentQueue<SocketMessage>>();
 }

 _should_stop = false;
}

void UdpServerNetworkManager::HandleRequestIn() {

  while (!_should_stop) {

    char buffer[SOCKET_MSG_BUF_SIZE];
    struct sockaddr_storage ss;
    char addr[INET6_ADDRSTRLEN];
    uint16_t port;

    struct sockaddr* sa = (struct sockaddr *)&ss;
    _listener->RecvFrom(buffer, SOCKET_MSG_BUF_SIZE, sa);
    ParseSockAddr(sa, addr, &port);

    auto request = NetworkRequest::FromString(buffer);
    auto ptrUdpSock = std::make_shared<UdpSocket>(addr, port);
    _queue_in->Push({request, ptrUdpSock});
  }
}

void UdpServerNetworkManager::HandleRequestOut() {

  while (!_should_stop) {

    SocketMessage message;
    if (_queue_out->Front(message) &&
        message.SptrSocket != nullptr && 
        message.SptrSocket->Type() == SOCK_DGRAM &&
        message.Request.GetRequestType() != NetworkRequest::RequestType::UNKNOWN) {

      _queue_out->Pop();
      auto request = message.Request.ToString();
      auto ptrUdpSock = std::dynamic_pointer_cast<UdpSocket>(message.SptrSocket);
      ptrUdpSock->SendTo(request.c_str(), request.size());
    }
  }
}

void UdpServerNetworkManager::Start() {
  _thread_in = new std::thread(&UdpServerNetworkManager::HandleRequestIn, this);
  _thread_out = new std::thread(&UdpServerNetworkManager::HandleRequestOut, this);
}
}