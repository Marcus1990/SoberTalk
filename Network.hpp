/*
*   Network support shared by both server and client. 
*   This contains TCP/UDP Socket model wrapping around C and various system calls
*
*   Author: Fu Qiao 
*   Email:  qiaofuphysics@gmail.com
*
*/

#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

#include <stdlib.h>
#include <errno.h>
#include <exception>
#include <string.h>
#include <string>
#include <sstream>

namespace network {

    //Throw socket related exceptions and expose details about errno.
    static inline std::string RaiseSocketException(const char* customMessage) {
        std::stringstream ss;
        ss << customMessage << " " << strerror(errno);
        throw SocketException(ss.str());
    }

    //Always declare sa as char[INET6_ADDRLEN]
    static inline void ParseSockAddr(const struct sockaddr* sa, char* addr, uint16_t* port) {
        if (sa->sa_family == AF_INET) {
            struct sockaddr_in* ipv4_addr = (struct sockaddr_in*)sa;
            inet_ntop(sa->sa_family, &(ipv4_addr->sin_addr), addr, sizeof(addr));
            *port = ipv4_addr->sin_port;
        }
        else {
            struct sockaddr_in6* ipv6_addr = (struct sockaddr_in6*)sa;
            inet_ntop(sa->sa_family, &(ipv6_addr->sin6_addr), addr, sizeof(addr));
            *port = ipv6_addr->sin6_port;
        }
    }

    class SocketException : public std::exception {

        private:
            std::string _message;

        public:
            explicit SocketException(const std::string& message) : _message(message) {}
            ~SocketException(){}
            const char*  what() { return _message.c_str(); }
    };

    /*
    Interface for socket system call is:

         int socket(int domain, int type, int protocol)

    Here we make use of system call getaddrinfo,and
     won't allow manually setting these values,
    */
    class Socket {

        private:

            Socket(const Socket& other);
            Socket& operator=(const Socket& other);

        public:
            
            int Descriptor() const { return _descriptor; }
            std::string Address() const { return _address; }
            int Port() const { return _port; }
            int Family() const { return _family; }
            int Type() const { return _type; }

        protected:

            int _descriptor;
            int _type;                      //SOCK_STREAM, SOCK_DGRAM
            int _family;                        //AF_INET, AF_INET6
            int _port;
            char _address[INET6_ADDRSTRLEN];
            struct addrinfo* _addrinfo;

            Socket(const char* address, int port, int stype, bool block = true)
                : _port(port) {

                struct addrinfo hints, *result, *p;

                memset(&hints, 0, sizeof(hints));
                hints.ai_family = AF_UNSPEC;
                hints.ai_socktype = stype;
                if (address == NULL)
                {
                    hints.ai_flags = AI_PASSIVE;   //fill local host address
                }

                int status = getaddrinfo(address, std::to_string(port).c_str(), &hints, &result);
                if (status != 0) {
                    RaiseSocketException("Error getaddrinfo: " );
                }

                int sockfd, yes = 1;
                for (p = result; p != NULL; p = p->ai_next)
                {
                    if (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) == -1) {
                        continue;
                    }
                    //Make this port reusable
                    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                        RaiseSocketException("Error when setsockopt: ");
                    }

                    _descriptor = sockfd;
                    if (address == NULL) { //auto bind if using local host
                        if (bind(_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
                            RaiseSocketException("Error when bind ");
                        }
                    }
                    break;
                }

                freeaddrinfo(result);

                if (p == NULL) {
                    RaiseSocketException("Error when creating socket: ");
                }

                _addrinfo = (struct addrinfo*)malloc(sizeof(struct addrinfo));
                _family = p->ai_family;

                if (_addrinfo == NULL) {
                    RaiseSocketException("Error when creating socket: ");
                }

                memcpy(_addrinfo, p, sizeof(struct addrinfo));
                inet_ntop(p->ai_family, p->ai_addr, _address, sizeof(_address));
            }

             ~Socket(){
                if (_descriptor != -1) {
                    close(_descriptor);
                    _descriptor = -1;
                }
                
                if (_addrinfo != NULL) {
                    free(_addrinfo);
                    _addrinfo = NULL;
                }
            }
    };

    class CommunicationSocket : public Socket {

        public:
            static CommunicationSocket* Connect(const char* remoteAddr, int remotePort, int stype = SOCK_STREAM) {
                CommunicationSocket* sock = new CommunicationSocket(remoteAddr, remotePort, stype);
                if (connect(sock->_descriptor, sock->_addrinfo->ai_addr, sock->_addrinfo->ai_addrlen) == -1) {
                    RaiseSocketException("Error when connect: ");
                }
            }

            CommunicationSocket(const char* addr, int port, int protocol, bool block = true)
                : Socket(addr, port, protocol, block) { }

            virtual ~CommunicationSocket() {}

            CommunicationSocket(const CommunicationSocket& other) = delete;
            CommunicationSocket& operator=(const CommunicationSocket& other) = delete;

            
            int Send(const void* buffer, int bufferLen) {
                int sent;
                if ((sent = send(_descriptor, buffer, bufferLen, 0)) == -1) {
                    RaiseSocketException("Error when send: ");
                }
                return sent;
            }

            //send all data in buffer and return true on success, otherwise false
            bool SendAll(const char* buffer, int bufferLen) {
                int sent = 0;
                int left = bufferLen;

                while (sent < bufferLen) {
                    int _n = Send(buffer + sent, left);
                    sent += _n;
                    left -= _n;
                }

                return sent == bufferLen ? true : false;
            }

            int Recv(void* buffer, int bufferLen) {
                int _recv = recv(_descriptor, buffer, bufferLen, 0);
                if (_recv == -1) {
                    RaiseSocketException("Error when recv: ");
                }
                return _recv;
            }
    };

    class TcpSocket : public CommunicationSocket {

        public:
            TcpSocket(const char* address, int port, bool block = true)
                : CommunicationSocket(address, port, SOCK_STREAM, block) {
                    
            }

            ~TcpSocket() {}

            TcpSocket(const TcpSocket& other) = delete;
            TcpSocket& operator=(const TcpSocket& other) = delete;

            void Listen(int backLog) {
                if (listen(_descriptor, backLog) == -1) {
                    RaiseSocketException("Error when listen: ");
                }
            }
    };

    class UdpSocket : public CommunicationSocket {

        public:
            UdpSocket(const char* address, int port, bool block = true)
                : CommunicationSocket(address, port, SOCK_DGRAM, block) {
            }

            ~UdpSocket(){}
            UdpSocket(const UdpSocket& other) = delete;
            UdpSocket& operator=(const UdpSocket& other) = delete;

            int SendTo(const void* buffer, int bufferLen, const struct sockaddr* sa) {
                int _sent = sendto(_descriptor, buffer, bufferLen, 0, sa, sizeof(struct sockaddr_storage));
                if (_sent == -1) {
                    RaiseSocketException("Error when sendto: ");
                }
                return _sent;
            }

            bool SendAllTo(const void* buffer, int bufferLen, const struct sockaddr* sa) {
                int sent = 0;
                int left = bufferLen;

                while (sent < bufferLen) {
                    int _n = SendTo(buffer + sent, left, sa);
                    sent += _n;
                    left -= _n;
                }
                return sent == bufferLen ? true : false;
            }

            int RecvFrom(void* buffer, int bufferLen, struct sockaddr* sa) {
                socklen_t remoteAddrSize = sizeof(struct sockaddr_storage);
                int _recv = recvfrom(_descriptor, buffer, bufferLen, 0, sa, &remoteAddrSize);

                if (_recv == -1) {
                    RaiseSocketException("Error when recvfrom: ");
                }

                return _recv;
            }
    };
}

#endif