#ifndef LIB_SOCKET_H
#define LIB_SOCKET_H

#if defined (_MSC_VER)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <string>
#include <jansson.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class socket_t
{
public:
  socket_t();
  socket_t(int socket_fd, sockaddr_in sock_addr);
  ~socket_t();
  void close();
  int write(const void *buf, int size_buf);
  int read_some(void *buf, int size_buf);
  int read_all_get_close(const char *file_name, bool verbose);
  std::string read_all_known_size(size_t size_read);
  int hostname_to_ip(const char *host_name, char *ip);

  //JSON functions
  int write(json_t *json);
  json_t * read();

public:
  int m_socket_fd; // socket descriptor 
  sockaddr_in m_sockaddr_in; // client address (used to store return value of server accept())
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class tcp_server_t : public socket_t
{
public:
  tcp_server_t(const unsigned short server_port);
  socket_t accept_client();
  ~tcp_server_t();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class tcp_client_t : public socket_t
{
public:
  tcp_client_t(const char *host_name, const unsigned short server_port);
  ~tcp_client_t();
  int open();

protected:
  std::string m_server_ip;
  unsigned short m_server_port;
};

#endif