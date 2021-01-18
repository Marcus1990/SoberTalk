#include "TcpServerNetworkManager.h"

namespace sobertalk {

TcpServerNetworkManager::TcpServerNetworkManager(uint16_t port,
                                                 std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, 
                                                 std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out)
  : NetworkServiceManager(queue_In, queue_Out), _port(port) {
  }

TcpServerNetworkManager::~TcpServerNetworkManager() {
}

void TcpServerNetworkManager::HandleRequestIn() {

  while (!_should_stop) {
    std::shared_ptr<TcpSocket> conn = std::make_shared<TcpSocket>(_listener->Accept());
    char buffer[SOCKET_MSG_BUF_SIZE];
    conn->Recv(buffer, SOCKET_MSG_BUF_SIZE);

    auto request = NetworkRequest::FromString(buffer);
    SocketMessage message {request, conn };
    _queue_in->Push(message);

  }
}

void TcpServerNetworkManager::HandleRequestOut() {

  while (!_should_stop) {
    SocketMessage message;
    if (_queue_out->Front(message) &&
        message.SptrSocket != nullptr &&
        message.Request.GetRequestType() != NetworkRequest::RequestType::UNKNOWN &&
        message.SptrSocket->Type() == SOCK_STREAM) {

        _queue_out->Pop();
        auto request = message.Request.ToString();
        message.SptrSocket->SendAll(request.c_str(), request.size());
    }
  }
}

void TcpServerNetworkManager::Init() {

  _listener = std::make_unique<TcpSocket>(NULL, _port);
  _listener->Listen();

  if (!_queue_in) {
   _queue_in = std::make_shared<ConcurrentQueue<SocketMessage>>();
  }

  if (!_queue_out) {
   _queue_out = std::make_shared<ConcurrentQueue<SocketMessage>>();
  }
  _should_stop = false;
}

void TcpServerNetworkManager::Start() {
  _thread_in = new std::thread(&TcpServerNetworkManager::HandleRequestIn, this);
  _thread_out = new std::thread(&TcpServerNetworkManager::HandleRequestOut, this);
}

}