#ifndef LIB_NETSOCKET_HTTP_H
#define LIB_NETSOCKET_HTTP_H
#include <string>
#include "socket.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class http_t: public tcp_client_t
{
public:
  http_t(const char *host_name, const unsigned short server_port);
  ~http_t();
  void get(const char *path_remote_file, bool verbose);

protected:
  void parse_headers();
  int m_header_len; // header lenght size in bytes
};




#endif
