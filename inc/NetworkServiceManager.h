/*
*   NetworkServiceManager define interfaces for network communication services
*
*   Author: Fu Qiao 
*   Email:  fqiao@protonmail.com
*
*/

#ifndef __NETWORK_SERVICE_MANAGER__
#define __NETWORK_SERVICE_MANAGER__

#include "Network.hpp"
#include "ConcurrentQueue.hpp"
#include "NetworkRequest.h"
#include <memory>
#include <thread>
#include <atomic>
#include <tuple>

namespace sobertalk {

using namespace common;

class NetworkServiceManager {

protected:
  std::shared_ptr<ConcurrentQueue<SocketMessage>> _queue_in;  

  std::shared_ptr<ConcurrentQueue<SocketMessage>> _queue_out;

  std::thread* _thread_in {NULL};

  std::thread* _thread_out {NULL};

  std::atomic<bool> _should_stop;

  virtual void Init() = 0;

  NetworkServiceManager(std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out);

  ~NetworkServiceManager();

  void SetStop();

public:
  
  virtual void HandleRequestOut() = 0;

  virtual void HandleRequestIn() = 0;

  virtual void Start() = 0;

  virtual void Stop();

private:

  NetworkServiceManager(const NetworkServiceManager& other);

  NetworkServiceManager& operator=(const NetworkServiceManager& other);

};

}
#endif