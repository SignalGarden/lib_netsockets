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
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "socket.hh"
#include "http.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//str_extract()
//extract last component of file full path
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string str_extract(const std::string &str_in)
{
  size_t pos = str_in.find_last_of("/\\");
  std::string str = str_in.substr(pos + 1, str_in.size());
  return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t::http_t()
/////////////////////////////////////////////////////////////////////////////////////////////////////

http_t::http_t(const char *host_name, const unsigned short server_port)
  : tcp_client_t(host_name, server_port),
  m_header_len(0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t::~http_t()
/////////////////////////////////////////////////////////////////////////////////////////////////////

http_t::~http_t()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t::get
/////////////////////////////////////////////////////////////////////////////////////////////////////

void http_t::get(const char *path_remote_file, bool verbose)
{
  char buf_request[1024];

  //open connection
  this->open();

  //construct request message using class input parameters
  sprintf(buf_request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path_remote_file, m_server_ip.c_str());

  std::cout << "get: " << buf_request << std::endl;

  //send request, using built in tcp_client_t socket
  this->write((void *)buf_request, strlen(buf_request));

  //parse headers
  this->parse_headers();

  //receive data and save to local file 
  std::string str_file_name = str_extract(path_remote_file);

  //we sent a close() server request, so we can use the read_all function
  //that checks for recv() return value of zero (connection closed)
  this->read_all(str_file_name.c_str(), verbose);

  //close connection
  this->close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t::parse_headers
//MSG_PEEK
//Peeks at an incoming message.The data is treated as unread and the next recv() or similar 
//function shall still return this data.
/////////////////////////////////////////////////////////////////////////////////////////////////////

void http_t::parse_headers()
{
  int recv_size; // size in bytes received or -1 on error 
  const int size_buf = 4096;
  char buf[size_buf];

  if ((recv_size = recv(m_socket, buf, size_buf, MSG_PEEK)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
  }

  std::string str(buf);
  size_t pos = str.find("\r\n\r\n");
  std::string str_headers(str.substr(0, pos + 4));
  m_header_len = static_cast<int>(pos + 4);

  std::cout << str_headers.c_str() << std::endl;

  //now get headers with the obtained size from socket
  if ((recv_size = recv(m_socket, buf, m_header_len, 0)) == -1)
  {
    std::cout << "recv error: " << strerror(errno) << std::endl;
  }

  //sanity check
  std::string str1(buf);
  assert(str1 == str);
  return;
}


