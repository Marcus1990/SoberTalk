/*
*   Commonly used data types shared by both server and client
*   Author: qiaofuphysics@gmail.com
*
*/

#ifndef __COMMON_DATA_TYPES_HPP__
#define __COMMON_DATA_TYPES_HPP__

#define TCP_PORT 8517   //TCP for message transmission
#define UDP_PORT 9487   //UDP for periodic status/new messages check
#define HEARTBEAT_RATE 5
#define SERVER_ADDRESS "linuxhost-0.westus.cloudapp.azure.com"


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

    enum class ClientStatus {

        ONLINE,

        OFFLINE
    };
}
#endif