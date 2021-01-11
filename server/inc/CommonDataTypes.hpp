/*
*   Commonly used data types shared by both server and client
*   Author: qiaofuphysics@gmail.com
*
*/

#ifndef __COMMON_DATA_TYPES_HPP__
#define __COMMON_DATA_TYPES_HPP__

#define SERVER_TCP_PORT 8517   //TCP for message transmission
#define SERVER_UDP_PORT 8964   //UDP for periodic status/new messages check
#define HEARTBEAT_RATE 5

#include <string>

namespace common {

enum class MessageType {

  CREATE_USER,

  DELETE_USER,

  PUSH_MESSAGE,

  POLL_MESSAGE,

  ADD_FRIEND,

  DELETE_FRIEND,

  REGULAR_CHECK,

  CHANGE_STATUS
};  

enum SocketType {

  TCP,

  UDP
};

enum class ClientStatus {

  ONLINE,

  OFFLINE
};

struct SocketMessage {

  SocketType SocketType;
  MessageType MessageType;
  std::string MessageContent;

};

}
#endif