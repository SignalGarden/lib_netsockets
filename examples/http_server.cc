#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "socket.hh"

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket);

int main(int argc, char *argv[])
{
  unsigned short port = 3000;

  for (int i = 1; i < argc && argv[i][0] == '-'; i++)
  {
    switch (argv[i][1])
    {
    case 'p':
      port = atoi(argv[i + 1]);
      i++;
      break;
    }
  }

  tcp_server_t server(port);
  std::cout << "server listening on port " << port << std::endl;
  while (true)
  {
    socket_t socket = server.accept_client();
    handle_client(socket);
    socket.close();
  }
  server.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//handle_client
/////////////////////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket)
{


}


