#ifndef LIB_NETSOCKET_HTTP_H
#define LIB_NETSOCKET_HTTP_H
#include <string>
#include "socket.hh"

const unsigned short http_port = 3000;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class http_t: public tcp_client_t
{
public:
  http_t(const char *host_name, const unsigned short server_port);
  ~http_t();
  int get(const char *path_remote_file, bool verbose);
};




#endif
