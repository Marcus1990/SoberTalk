#include "SoberTalkApp.h"
#include "Common.hpp"
#include <exception>
#include <

namespace sobertalk {

SoberTalkApp::SoberTalkApp() {

_queue_In = std::make_shared<SocketMessageQueue>();
_queue_Out = std::make_shared<SocketMessageQueue>();

_tcpManager = std::make_unique<TcpServerNetworkManager>(SERVER_TCP_PORT, _queue_In, _queue_Out);
_udpManager = std::make_unique<UdpServerNetworkManager>(SERVER_UDP_PORT, _queue_In, _queue_Out);

}

SoberTalkApp::~SoberTalkApp() {
  _tcpManager->Stop();
  _udpManager->Stop();
}

void SoberTalkApp::Run() {
  _tcpManager->Start();
  _udpManager->Start();
}

void SoberTalkApp::ProcessNetworkRequest() {
  using RequestType = common::NetworkRequest::RequestType;

  while (true) {

    SocketMessage message;
    if (_queue_In->Front(message)) {
      
      _queue_In->Pop();

      switch (message.Request.GetRequestType()) {
        case RequestType::CREATE_USER:

          break;

        case RequestType::DELETE_USER:

          break;

        case RequestType::PUSH_MESSAGE:

          break;

        case RequestType::POLL_MESSAGE:

          break;

        case RequestType::ADD_FRIEND:

          break;

        case RequestType::DELETE_FRIEND:

          break;

        case RequestType::REGULAR_CHECK:

          break;

        case RequestType::CHANGE_STATUS:

          break;

        default:
          std::stringstream ss;
          ss << "Unknown network request type for server. Request type code : " << (int)message.Request.GetRequestType();
          throw std::invalid_argument(ss.str().c_str());
      }
    }
  }
}
}