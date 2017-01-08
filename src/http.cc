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
  : tcp_client_t(host_name, server_port)
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

int http_t::get(const char *path_remote_file, bool verbose)
{
  char buf_request[1024];

  //construct request message using class input parameters
  sprintf(buf_request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path_remote_file, m_server_ip.c_str());

  //send request, using built in tcp_client_t socket
  if (this->write(buf_request, strlen(buf_request)) < 0)
  {
    return -1;
  }

  std::cout << "request: " << buf_request << std::endl;

  //parse headers
  if (parse_http_headers() < 0)
  {
    return -1;
  }

  //receive data and save to local file 
  std::string str_file_name = str_extract(path_remote_file);

  //we sent a close() server request, so we can use the read_all function
  //that checks for recv() return value of zero (connection closed)
  this->read_all_get_close(str_file_name.c_str(), verbose);

  return 0;
}



