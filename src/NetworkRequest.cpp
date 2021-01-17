#include "NetworkRequest.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

namespace common {

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

NetworkRequest::NetworkRequest(const std::string& parameters, RequestType rtype)
 : _parameters(parameters), _rtype(rtype) {
}

NetworkRequest::~NetworkRequest() {}

NetworkRequest::RequestType NetworkRequest::GetRequestType() const { return _rtype; }

const std::string& NetworkRequest::GetParameters() const { return _parameters; }

std::string NetworkRequest::ToString() const {
 ptree pt;
 std::ostringstream oss;
 pt.put("request_type", static_cast<int>(_rtype));
 pt.put("parameters", _parameters);

 write_json(oss, pt, false);
 return oss.str();
}

NetworkRequest NetworkRequest::FromString(const std::string& request) {
 ptree pt;
 std::istringstream iss {request};
 read_json(iss, pt);
 auto rtype = static_cast<NetworkRequest::RequestType>(pt.get<int>("request_type"));
 auto parameters = pt.get<std::string>("parameters");
 return NetworkRequest(parameters, rtype);

}
}