/*
*   NetworkServiceManager define interfaces for network communication services
*
*   Author: Fu Qiao 
*   Email:  qiaofuphysics@gmail.com
*
*/

#ifndef __NETWORK_SERVICE_MANAGER__
#define __NETWORK_SERVICE_MANAGER__

#include "Network.hpp"
#include "ConcurrentQueue.hpp"
#include "CommonDataTypes.hpp"
#include <memory>
#include <thread>

using namespace common;
using namespace network;

namespace sobertalk {

class NetworkServiceManager {

protected:
  std::shared_ptr<ConcurrentQueue<SocketMessage>> _queue_in;  

  std::shared_ptr<ConcurrentQueue<SocketMessage>> _queue_out;

  std::thread* _thread_in {NULL};

  std::thread* _thread_out {NULL};

  virtual void Init() = 0;

  NetworkServiceManager(std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_In, std::shared_ptr<ConcurrentQueue<SocketMessage>> queue_Out);

  ~NetworkServiceManager();

public:
  
  virtual void SendMessage(const SocketMessage& msg) = 0;

  virtual void RecvMessage() = 0;

  virtual void Start() = 0;

  virtual void Stop() = 0;

private:

  NetworkServiceManager(const NetworkServiceManager& other);

  NetworkServiceManager& operator=(const NetworkServiceManager& other);

};

}
#endif