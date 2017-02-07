#if defined (_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> //hostent
#endif

#include <iostream>
#include <cerrno>
#include <string>
#include <stdio.h>
#include <string.h>

#if _ANDROID
#include <sstream>
#endif

#include <iostream>
#include <stdio.h>
#include <jansson.h>
#include <assert.h>
#include <time.h>
#include <ctime>
#include "socket.hh"

#if defined (_MSC_VER)
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#endif

const int MAXPENDING = 5; // maximum outstanding connection requests

/////////////////////////////////////////////////////////////////////////////////////////////////////
//utils
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string prt_time()
{
  time_t t;
  time(&t);
  std::string str_time(std::ctime(&t));
  str_time.resize(str_time.size() - 1); //remove \n
  str_time += " ";
  return str_time;
}

///////////////////////////////////////////////////////////////////////////////////////
//set_daemon
///////////////////////////////////////////////////////////////////////////////////////

int set_daemon(const char* str_dir)
{
#if defined (_MSC_VER)
  std::cout << str_dir << std::endl;
#else
  pid_t pid, sid;
  pid = fork();
  if (pid < 0)
  {
    std::cout << "cannot create pid: " << pid << std::endl;
    exit(EXIT_FAILURE);
  }
  if (pid > 0)
  {
    std::cout << "created pid: " << pid << std::endl;
    exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();
  if (sid < 0)
  {
    std::cout << "cannot create sid: " << sid << std::endl;
    exit(EXIT_FAILURE);
  }

  if ((chdir(str_dir)) < 0)
  {
    std::cout << "cannot chdir to: " << str_dir << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "chdir to: " << str_dir << std::endl;

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
#endif
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::socket_t()
/////////////////////////////////////////////////////////////////////////////////////////////////////

socket_t::socket_t() : m_socket_fd(-1)
{
  memset(&m_sockaddr_in, 0, sizeof(m_sockaddr_in));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::socket_t()
/////////////////////////////////////////////////////////////////////////////////////////////////////

socket_t::socket_t(int socket_fd, sockaddr_in sock_addr) :
  m_socket_fd(socket_fd),
  m_sockaddr_in(sock_addr)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::~socket_t()
/////////////////////////////////////////////////////////////////////////////////////////////////////

socket_t::~socket_t()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::close()
/////////////////////////////////////////////////////////////////////////////////////////////////////

void socket_t::close()
{
#if defined (_MSC_VER)
  closesocket(m_socket_fd);
#else
  ::close(m_socket_fd);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::write
/////////////////////////////////////////////////////////////////////////////////////////////////////

int socket_t::write(const void *_buf, int size_buf)
{
  const char *buf = (char*)_buf; // can't do pointer arithmetic on void* 
  int send_size; // size in bytes sent or -1 on error 
  size_t size_left; // size in bytes left to send 
  const int flags = 0;

  size_left = size_buf;

  while (size_left > 0)
  {
    if ((send_size = send(m_socket_fd, buf, size_left, flags)) == -1)
    {
      std::cout << "send error: " << strerror(errno) << std::endl;
      return -1;
    }
    size_left -= send_size;
    buf += send_size;
  }

  return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::read_some
//assumptions: 
//total size to receive is not known
//other end did not close() connection, so not possible to check a zero return value of recv()
/////////////////////////////////////////////////////////////////////////////////////////////////////

int socket_t::read_some(void *buf, int size_buf)
{
  int recv_size; // size in bytes received or -1 on error 
  const int flags = 0;

  if ((recv_size = recv(m_socket_fd, static_cast<char *>(buf), size_buf, flags)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
  }

  return recv_size;
}

///////////////////////////////////////////////////////////////////////////////////////
//socket_t::read_all_get_close
//http://man7.org/linux/man-pages/man2/recv.2.html
//recv() 'size_buf' bytes and save to local FILE
//assumptions: 
//total size to receive is not known
//other end did a close() connection, so it is possible to check a zero return value of recv()
//usage in HTTP
///////////////////////////////////////////////////////////////////////////////////////

int socket_t::read_all_get_close(const char *file_name, bool verbose)
{
  int recv_size; // size in bytes received or -1 on error 
  int total_recv_size = 0;
  const int flags = 0;
  const int size_buf = 4096;
  char buf[size_buf];
  FILE *file;

  file = fopen(file_name, "wb");
  while (1)
  {
    if ((recv_size = recv(m_socket_fd, buf, size_buf, flags)) == -1)
    {
      std::cout << "recv error: " << strerror(errno) << std::endl;
    }
    total_recv_size += recv_size;
    if (recv_size == 0)
    {
      std::cout << "all bytes received " << std::endl;
      break;
    }
    if (verbose)
    {
      for (int i = 0; i < recv_size; i++)
      {
        std::cout << buf[i];
      }
    }
    fwrite(buf, recv_size, 1, file);
  }
  fclose(file);
  return total_recv_size;
}

///////////////////////////////////////////////////////////////////////////////////////
//socket_t::read_all_known_size
//assumptions: 
//total size to receive is known, size_read
///////////////////////////////////////////////////////////////////////////////////////

std::string socket_t::read_all_known_size(size_t size_read)
{
  const int size_buf = size_read;
  char *buf = new char[size_buf];
  int recv_size; // size in bytes received or -1 on error 
  size_t size_left; // size in bytes left to receive 
  const int flags = 0;

  size_left = size_read;

  std::string str_ret;
  while (size_left > 0)
  {
    if ((recv_size = recv(m_socket_fd, buf, size_buf, flags)) == -1)
    {
      std::cout << "recv error: " << strerror(errno) << std::endl;
    }
    size_left -= recv_size;
    std::string str(buf);
    //terminate buffer with received size
    str.resize(recv_size);
    //export buffer
    str_ret += str;
  }

  delete[] buf;
  return str_ret;
}

///////////////////////////////////////////////////////////////////////////////////////
//socket_t::hostname_to_ip
//The getaddrinfo function provides protocol-independent translation from an ANSI host name to an address
///////////////////////////////////////////////////////////////////////////////////////

int socket_t::hostname_to_ip(const char *host_name, char *ip)
{
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *h;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if ((rv = getaddrinfo(host_name, "http", &hints, &servinfo)) != 0)
  {
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    h = (struct sockaddr_in *) p->ai_addr;
    strcpy(ip, inet_ntoa(h->sin_addr));
  }

  freeaddrinfo(servinfo);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::write_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

#if _ANDROID
namespace std
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}
#endif

int socket_t::write_json(json_t *json)
{
  char *buf_json = NULL;
  std::string buf_send;
  size_t size_json;

  //get char* from json_t
  buf_json = json_dumps(json, JSON_PRESERVE_ORDER & JSON_COMPACT);
  size_json = strlen(buf_json);

  //construct send buffer, adding a header with size in bytes of JSON and # terminator
  buf_send = std::to_string(static_cast<long long unsigned int>(size_json));
  buf_send += "#";
  buf_send += std::string(buf_json);
  free(buf_json);
  return (this->write(buf_send.data(), buf_send.size()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//json_socket_t::read_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

json_t * socket_t::read_json()
{
  int recv_size; // size in bytes received or -1 on error 
  size_t size_json = 0; //in bytes
  std::string str_header;

  //parse header, one character at a time and look for for separator #, assume size header lenght less than 20 digits
  for (size_t idx = 0; idx < 20; idx++)
  {
    char c;
    if ((recv_size = recv(m_socket_fd, &c, 1, 0)) == -1)
    {
      std::cout << "recv error: " << strerror(errno) << std::endl;
      return NULL;
    }
    if (c == '#')
    {
      break;
    }
    else
    {
      str_header += c;
    }
  }

  //get size
  size_json = static_cast<size_t>(atoi(str_header.c_str()));

  //read from socket with known size
  std::string str_json = read_all_known_size(size_json);

  //construct and return JSON (c_str() is NULL terminated, JSON_DISABLE_EOF_CHECK must be set)
  json_error_t *err = NULL;
  json_t *json = json_loads(str_json.c_str(), JSON_DISABLE_EOF_CHECK, err);
  return json;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::tcp_server_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_server_t::tcp_server_t(const unsigned short server_port)
  : socket_t()
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
  if ((m_socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
  if (bind(m_socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
  {
    //bind error: Permission denied
    //probably trying to bind a port under 1024. These ports usually require root privileges to be bound.
    std::cout << "bind error: " << std::endl;
    exit(1);
  }

  // mark the socket so it will listen for incoming connections
  if (listen(m_socket_fd, MAXPENDING) < 0)
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
  int socket_client_fd; // socket descriptor for client
  sockaddr_in addr_client; // client address
#if defined (_MSC_VER)
  int len_addr; // length of client address data structure
#else
  socklen_t len_addr;
#endif

  // set length of client address structure (in-out parameter)
  len_addr = sizeof(addr_client);

  // wait for a client to connect
  if ((socket_client_fd = accept(m_socket_fd, (struct sockaddr *) &addr_client, &len_addr)) < 0)
  {
    std::cout << "accept error " << std::endl;
  }

  socket_t socket(socket_client_fd, addr_client);
  return socket;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t::tcp_client_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

tcp_client_t::tcp_client_t(const char *host_name, const unsigned short server_port)
  : socket_t(),
  m_server_port(server_port)
{
#if defined (_MSC_VER)
  WSADATA ws_data;
  if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
  {
    exit(1);
  }
#endif

  char server_ip[100];

  //get ip address from hostname
  hostname_to_ip(host_name, server_ip);

  //store
  m_server_ip = server_ip;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_client_t::open
/////////////////////////////////////////////////////////////////////////////////////////////////////

int tcp_client_t::open()
{
  struct sockaddr_in server_addr; // server address

  // create a stream socket using TCP
  if ((m_socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    std::cout << "socket error: " << std::endl;
    return -1;
  }

  // construct the server address structure
  memset(&server_addr, 0, sizeof(server_addr)); // zero out structure
  server_addr.sin_family = AF_INET; // internet address family
  if (inet_pton(AF_INET, m_server_ip.c_str(), &server_addr.sin_addr) <= 0) // server IP address
  {
    std::cout << "inet_pton error: " << strerror(errno) << std::endl;
    return -1;
  }
  server_addr.sin_port = htons(m_server_port); // server port

  // establish the connection to the server
  if (connect(m_socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  {
    std::cout << "connect error: " << strerror(errno) << std::endl;
    return -1;
  }
  return 0;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//socket_t::parse_http_headers
//MSG_PEEK
//Peeks at an incoming message.The data is treated as unread and the next recv() or similar 
//function shall still return this data.
/////////////////////////////////////////////////////////////////////////////////////////////////////

int socket_t::parse_http_headers()
{
  int recv_size; // size in bytes received or -1 on error 
  const int size_buf = 4096;
  char buf[size_buf];

  if ((recv_size = recv(m_socket_fd, buf, size_buf, MSG_PEEK)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
    return -1;
  }

  std::string str(buf);
  size_t pos = str.find("\r\n\r\n");
  std::string str_headers(str.substr(0, pos + 4));
  int header_len = static_cast<int>(pos + 4);

  std::cout << str_headers.c_str() << std::endl;

  //now get headers with the obtained size from socket
  if ((recv_size = recv(m_socket_fd, buf, header_len, 0)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
    return -1;
  }

  //sanity check
  std::string str1(buf);
  assert(str1 == str);
  return 0;
}



