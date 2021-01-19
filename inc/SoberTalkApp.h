/*
*   The top level class handling the entire backend of SoberTalk
*
*   Author: Fu Qiao 
*   Email:  fqiao@protonmail.com
*
*/

#ifndef __SOBER_TALK_APP_H__
#define __SOBER_TALK_APP_H__

#include "TcpServerNetworkManager.h"
#include "UdpServerNetworkManager.h"

namespace sobertalk {

class SoberTalkApp final {

using SocketMessage = common::SocketMessage;
using SocketMessageQueue = common::ConcurrentQueue<SocketMessage>;

private:
 std::unique_ptr<TcpServerNetworkManager> _tcpManager;
 std::unique_ptr<UdpServerNetworkManager> _udpManager;
 std::shared_ptr<SocketMessageQueue> _queue_In {nullptr};
 std::shared_ptr<SocketMessageQueue> _queue_Out {nullptr};

 void ProcessNetworkRequest();

public:
 SoberTalkApp();
 ~SoberTalkApp();

 SoberTalkApp(const SoberTalkApp& other) = delete;
 SoberTalkApp& operator=(const SoberTalkApp& other) = delete;

 void Run();
};
}

#endif