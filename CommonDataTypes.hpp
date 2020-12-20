/*
*   Commonly used data types shared by both server and client
*   Author: qiaofuphysics@gmail.com
*
*/

#ifndef __COMMON_DATA_TYPES_HPP__
#define __COMMON_DATA_TYPES_HPP__

namespace common {

    // All types of messages that clients send to server
    enum class ClientMessageTyp {

        REGISTRATION,

        LOGIN,

        LOGOUT,

        PULL,

        POST
    };

    enum class ClientStatusType{
        
        ONLINE,

        OFFLINE
    };
}
#endif