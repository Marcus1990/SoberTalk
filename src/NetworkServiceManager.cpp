#include "Common.hpp"
#include "NetworkServiceManager.h"

namespace sobertalk {

NetworkServiceManager::NetworkServiceManager(std::shared_ptr<SocketMessageQueue> queue_In, std::shared_ptr<SocketMessageQueue> queue_Out) {
  Init();
}

NetworkServiceManager::~NetworkServiceManager() {
  SetStop();

  if (_thread_in ) {
    if (_thread_in->joinable()) {
      _thread_in->join();
    }

    delete _thread_in;
  }

  if (_thread_out) {
    if (_thread_out->joinable()) {
      _thread_out->join();
    }

    delete _thread_out;
  }
}

void NetworkServiceManager::Stop() {
  SetStop();
}

void NetworkServiceManager::SetStop() {
    _should_stop = true;
}

}