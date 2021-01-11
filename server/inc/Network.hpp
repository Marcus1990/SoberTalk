/*
*   Network support shared by both server and client. 
*   This contains TCP/UDP Socket model wrapping around C based system calls
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

  class SocketException : public std::exception {

  private:
    std::string _message;

  public:
    explicit SocketException(const std::string &message) : _message(message) {}
    ~SocketException() {}
    const char *what() { return _message.c_str(); }
  };

    //Throw socket related exceptions and expose details about errno.
  static inline std::string RaiseSocketException(const char *customMessage) {
    std::stringstream ss;
    ss << customMessage << " " << strerror(errno);
    throw SocketException(ss.str());
  }

  //Always make addr as char[INET6_ADDRSTRLEN];
  static inline void ParseSockAddr(const struct sockaddr *sa, char *addr, uint16_t *port) {
    if (sa->sa_family == AF_INET)
    {
      struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)sa;
      inet_ntop(sa->sa_family, &(ipv4_addr->sin_addr), addr, INET6_ADDRSTRLEN);
      *port = ntohs(ipv4_addr->sin_port); // convert to host byte order
    }
    else
    {
      struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)sa;
      inet_ntop(sa->sa_family, &(ipv6_addr->sin6_addr), addr, INET6_ADDRSTRLEN);
      *port = ntohs(ipv6_addr->sin6_port); // convert to host byte order
    }
  }

    /*
    Interface for socket system call is:

         int socket(int domain, int type, int protocol)

    Here we make use of system call getaddrinfo, to skip manually setting these values
    */
  class Socket {

  private:
    int _type;   //SOCK_STREAM, SOCK_DGRAM
    int _family; //AF_INET, AF_INET6
    uint16_t _port;
    std::string _ip_address;

    Socket(const Socket &other);
    Socket &operator=(const Socket &other);

    void SaveSockAddr(const struct sockaddr *raw_sockaddr) {
      if (_sockaddr != NULL)
      {
        memset(_sockaddr, 0, sizeof(struct sockaddr));
      }
      else
      {
        _sockaddr = (struct sockaddr *)malloc(sizeof(struct sockaddr));
        if (_sockaddr == NULL)
        {
          RaiseSocketException("Error when saving sockaddr: ");
        }
      }
      memcpy(_sockaddr, raw_sockaddr, sizeof(struct sockaddr));
    }

  public:
    int Descriptor() const { return _descriptor; }
    int Family() const { return _family; }
    int Type() const { return _type; }
    std::string Address() const { return _ip_address; }
    uint16_t Port() const { return _port; }

  protected:
    int _descriptor;
    struct sockaddr *_sockaddr;
    socklen_t _addrlen;

    Socket(const char *address, uint16_t port, int stype, bool block = true) : _port(port) {

      struct addrinfo hints, *result, *p;

      memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = stype;
      if (address == NULL)
      {
        hints.ai_flags = AI_PASSIVE; //fill local host address
      }

      int status = getaddrinfo(address, std::to_string(port).c_str(), &hints, &result);
      if (status != 0)
      {
        RaiseSocketException("Error getaddrinfo: ");
      }

      int sockfd, yes = 1;
      char _ip_addr_str[INET6_ADDRSTRLEN];
      for (p = result; p != NULL; p = p->ai_next)
      {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
          continue;
        }
        //Make this port reusable
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
          RaiseSocketException("Error when setsockopt: ");
        }

        _descriptor = sockfd;
        if (address == NULL)
        { //auto bind if using local host
          if (bind(_descriptor, p->ai_addr, p->ai_addrlen) == -1)
          {
            RaiseSocketException("Error when bind ");
          }
        }

        ParseSockAddr(p->ai_addr, _ip_addr_str, &_port);
        SaveSockAddr(p->ai_addr);
        _ip_address = _ip_addr_str;
        _family = p->ai_family;
        _addrlen = p->ai_addrlen;

        break;
      }

      freeaddrinfo(result);
    }

    //Used by Accept only
    Socket(int descriptor, const struct sockaddr *raw_sockaddr, int stype = SOCK_STREAM)
        : _descriptor(descriptor), _type(stype) {

      char _ip_addr_str[INET6_ADDRSTRLEN];
      ParseSockAddr(raw_sockaddr, _ip_addr_str, &_port);
      _ip_address = _ip_addr_str;
      _family = raw_sockaddr->sa_family;
      _addrlen = _family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
      SaveSockAddr(raw_sockaddr);
    }

    ~Socket() {
      if (_descriptor != -1)
      {
        close(_descriptor);
        _descriptor = -1;
      }

      if (_sockaddr != NULL)
      {
        free(_sockaddr);
        _sockaddr = NULL;
      }
    }
  };

  class CommunicationSocket : public Socket {

  public:
    static CommunicationSocket *Connect(const char *remoteAddr, uint16_t remotePort, int stype = SOCK_STREAM) {
      CommunicationSocket *sock = new CommunicationSocket(remoteAddr, remotePort, stype);
      if (connect(sock->_descriptor, sock->_sockaddr, sock->_addrlen) == -1)
      {
        RaiseSocketException("Error when connect: ");
      }

      return sock;
    }

  protected:
    CommunicationSocket(const char *addr, uint16_t port, int stype, bool block = true)
        : Socket(addr, port, stype, block) {}

    ~CommunicationSocket() {}

    CommunicationSocket(const CommunicationSocket &other) = delete;
    CommunicationSocket &operator=(const CommunicationSocket &other) = delete;

  public:
    int Send(const void *buffer, int bufferLen) {
      int sent;
      if ((sent = send(_descriptor, buffer, bufferLen, 0)) == -1)
      {
        RaiseSocketException("Error when send: ");
      }
      return sent;
    }

    //send all data in buffer and return true on success, otherwise false
    bool SendAll(const char *buffer, int bufferLen) {
      int sent = 0;
      int left = bufferLen;

      while (sent < bufferLen)
      {
        int _n = Send(buffer + sent, left);
        sent += _n;
        left -= _n;
      }

      return sent == bufferLen ? true : false;
    }

    int Recv(void *buffer, int bufferLen) {
      int _recv = recv(_descriptor, buffer, bufferLen, 0);
      if (_recv == -1)
      {
        RaiseSocketException("Error when recv: ");
      }
      return _recv;
    }

  protected:
    CommunicationSocket(int descriptor, const struct sockaddr *raw_sockaddr, int stype)
        : Socket(descriptor, raw_sockaddr, stype) {
    }
  };

  class TcpSocket : public CommunicationSocket {

  public:
    TcpSocket(const char *address, uint16_t port, bool block = true)
        : CommunicationSocket(address, port, SOCK_STREAM, block) {
    }

    ~TcpSocket() {}

    TcpSocket(const TcpSocket &other) = delete;
    TcpSocket &operator=(const TcpSocket &other) = delete;

    void Listen(int backLog = SOMAXCONN) {
      if (listen(_descriptor, backLog) == -1)
      {
        RaiseSocketException("Error when listen: ");
      }
    }

    TcpSocket *Accept() {
      struct sockaddr_storage remoteAddr;
      socklen_t remoteAddrSize = sizeof(remoteAddr);
      struct sockaddr *sockAddr = (struct sockaddr *)&remoteAddr;

      int new_fd = accept(_descriptor, sockAddr, &remoteAddrSize);
      if (new_fd == -1)
      {
        RaiseSocketException("Error when accept: ");
      }

      return new TcpSocket(new_fd, sockAddr);
    }

  private:
    TcpSocket(int descriptor, const struct sockaddr *raw_sockaddr)
        : CommunicationSocket(descriptor, raw_sockaddr, SOCK_STREAM) {
    }
  };

  class UdpSocket : public CommunicationSocket {

  public:
    UdpSocket(const char *address, int port, bool block = true)
        : CommunicationSocket(address, port, SOCK_DGRAM, block) {
    }

    ~UdpSocket() {}
    UdpSocket(const UdpSocket &other) = delete;
    UdpSocket &operator=(const UdpSocket &other) = delete;

    int SendTo(const void *buffer, int bufferLen, const struct sockaddr *sa) {
      int _sent = sendto(_descriptor, buffer, bufferLen, 0, sa, sizeof(struct sockaddr_storage));
      if (_sent == -1)
      {
        RaiseSocketException("Error when sendto: ");
      }
      return _sent;
    }

    bool SendAllTo(const char *buffer, int bufferLen, const struct sockaddr *sa) {
      int sent = 0;
      int left = bufferLen;

      while (sent < bufferLen)
      {
        int _n = SendTo(buffer + sent, left, sa);
        sent += _n;
        left -= _n;
      }
      return sent == bufferLen ? true : false;
    }

    int RecvFrom(void *buffer, int bufferLen, struct sockaddr *sa) {
      socklen_t remoteAddrSize = sizeof(struct sockaddr_storage);
      int _recv = recvfrom(_descriptor, buffer, bufferLen, 0, sa, &remoteAddrSize);

      if (_recv == -1)
      {
        RaiseSocketException("Error when recvfrom: ");
      }

      return _recv;
    }
  };
}

#endif