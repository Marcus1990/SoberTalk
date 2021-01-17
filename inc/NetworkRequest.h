#ifndef __NETWORK_REQUEST__
#define __NETWORK_REQUEST__

#include "Network.hpp"
#include <string>
#include <memory>

namespace common {

class NetworkRequest {

public:

enum class RequestType {

  UNKNOWN,

  CREATE_USER,

  DELETE_USER,

  PUSH_MESSAGE,

  POLL_MESSAGE,

  ADD_FRIEND,

  DELETE_FRIEND,

  REGULAR_CHECK,

  CHANGE_STATUS
};

NetworkRequest(const std::string& parameters = "", RequestType rtype = RequestType::UNKNOWN);
~NetworkRequest();

NetworkRequest(const NetworkRequest& other) = default;
NetworkRequest& operator=(const NetworkRequest& other) = default;

RequestType GetRequestType() const;
const std::string& GetParameters() const;

std::string ToString() const;
static NetworkRequest FromString(const std::string& request);

private:

 RequestType _rtype;
 std::string _parameters;
};

struct SocketMessage {

 NetworkRequest Request;
 std::shared_ptr<network::CommunicationSocket> SptrSocket;
};
}
#endif