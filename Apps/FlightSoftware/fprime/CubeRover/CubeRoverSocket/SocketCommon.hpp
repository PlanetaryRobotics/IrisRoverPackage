#ifndef __SOCKET_COMMON_HPP__
#define __SOCKET_COMMON_HPP__

namespace Socket{
  const static uint32_t MAX_ADDRESS_SIZE = 64;
  const static uint32_t UDP_RECEIVER_MSG_SIZE = 256;

  #define SCI_REG sciREG

  typedef enum SocketFlag{
    MSG_CMSG_CLOEXEC = 0x01,
    MSG_DONTWAIT   = 0x02,
    MSG_ERRQUEUE   = 0x04,
    MSG_OOB      = 0x08,
    MSG_PEEK     = 0x10,
    MSG_TRUNC    = 0x20,
    MSG_WAITALL    = 0x40
  }SocketFlag;

  typedef enum SocketAddressFamily{
    AF_LOCAL,   // Local ommunication
    AF_INET,    // IPv4 Internet protocols
    AF_INET6,   // IPv6 Internet protocols
  }SocketAddressFamily;

  typedef struct SockAddr{
    SocketAddressFamily family;     // socket address family
    uint32_t  port;                 // port
    char sa_data[MAX_ADDRESS_SIZE]; // socket address (variable-length data)
  }SockAddr;

  typedef enum SocketError{
    EAGAIN,
    EWOULDBLOCK,
    EBADF,
    ECONNREFUSED,
    EFAULT,
    EINVAL,
    ENOMEM,
    ENOTCONN,
    ENOTSOCK
  }SocketError;

  typedef uint32_t SockLength;
};

#endif //__SOCKET_COMMON_HPP__
