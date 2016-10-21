#if defined (_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "socket.hh"

const int MAXPENDING = 5; // maximum outstanding connection requests

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::close()
/////////////////////////////////////////////////////////////////////////////////////////////////////

void socket_t::close()
{
#if defined (_MSC_VER)
  closesocket(m_socket);
#else
  ::close(m_socket);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::write
/////////////////////////////////////////////////////////////////////////////////////////////////////

void socket_t::write(const void *_buf, int size_buf)
{
  const char *buf = (char*)_buf;	// can't do pointer arithmetic on void* 
  int send_size; // size in bytes sent or -1 on error 
  size_t size_left; // size in bytes left to send 
  const int flags = 0;

  size_left = size_buf;

  while (size_left > 0)
  {
    if ((send_size = send(m_socket, buf, size_left, flags)) == -1)
    {
      std::cout << "send error: " << strerror(errno) << std::endl;
    }
    size_left -= send_size;
    buf += send_size;
  }

  return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::read
//assumptions: total size to receive is not known
/////////////////////////////////////////////////////////////////////////////////////////////////////

int socket_t::read(void *buf, int size_buf)
{
  int recv_size; // size in bytes received or -1 on error 
  const int flags = 0;

  if ((recv_size = recv(m_socket, static_cast<char *>(buf), size_buf, flags)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
  }

  return recv_size;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::tcp_server_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_server_t::tcp_server_t(const unsigned short server_port)
  : socket_t(-1)
{
#if defined (_MSC_VER)
  WSADATA ws_data;
  if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
  {
    exit(1);
  }
#endif
  sockaddr_in server_addr; // local address

  // create TCP socket for incoming connections
  if ((m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    std::cout << "socket error: " << std::endl;
    exit(1);
  }

  // construct local address structure
  memset(&server_addr, 0, sizeof(server_addr));     // zero out structure
  server_addr.sin_family = AF_INET;                 // internet address family
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // any incoming interface
  server_addr.sin_port = htons(server_port);        // local port

  // bind to the local address
  if (bind(m_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
  {
    //bind error: Permission denied
    //probably trying to bind a port under 1024. These ports usually require root privileges to be bound.
    std::cout << "bind error: " << std::endl;
    exit(1);
  }

  // mark the socket so it will listen for incoming connections
  if (listen(m_socket, MAXPENDING) < 0)
  {
    std::cout << "listen error: " << std::endl;
    exit(1);
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::~tcp_server_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_server_t::~tcp_server_t()
{
#if defined (_MSC_VER)
  WSACleanup();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::accept
/////////////////////////////////////////////////////////////////////////////////////////////////////

socket_t tcp_server_t::accept_client()
{
  int client_socket; // socket descriptor for client
  sockaddr_in client_addr; // client address
#if defined (_MSC_VER)
  int len_addr; // length of client address data structure
#else
  socklen_t len_addr;
#endif

  // set length of client address structure (in-out parameter)
  len_addr = sizeof(client_addr);

  // wait for a client to connect
  if ((client_socket = accept(m_socket, (struct sockaddr *) &client_addr, &len_addr)) < 0)
  {
    std::cout << "accept error " << std::endl;
  }

  // convert IP addresses from a dots-and-number string to a struct in_addr and back
  char *str_ip = inet_ntoa(client_addr.sin_addr);
  std::cout << "handling client " << str_ip << std::endl;

  socket_t socket(client_socket);
  return socket;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t::tcp_client_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_client_t::tcp_client_t(const char *server_ip, const unsigned short server_port)
  : socket_t(-1)
{
#if defined (_MSC_VER)
  WSADATA ws_data;
  if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
  {
    exit(1);
  }
#endif

  struct sockaddr_in server_addr; // server address

  // create a stream socket using TCP
  if ((m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    std::cout << "socket error: " << std::endl;
    exit(1);
  }

  // construct the server address structure
  memset(&server_addr, 0, sizeof(server_addr));          // zero out structure
  server_addr.sin_family = AF_INET;                      // internet address family
  server_addr.sin_addr.s_addr = inet_addr(server_ip);    // server IP address
  server_addr.sin_port = htons(server_port);             // server port

  // establish the connection to the server
  if (connect(m_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  {
    std::cout << "connect error: " << std::endl;
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t::~tcp_client_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_client_t::~tcp_client_t()
{
#if defined (_MSC_VER)
  WSACleanup();
#endif
}
