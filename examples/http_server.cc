#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "socket.hh"
#include "http.hh"

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket);

int main(int argc, char *argv[])
{
  unsigned short port = http_port;

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

    // convert IP addresses from a dots-and-number string to a struct in_addr
    char *str_ip = inet_ntoa(socket.m_sockaddr_in.sin_addr);
    std::cout << prt_time() << "server accepted: " << str_ip << "," << socket.m_socket_fd << std::endl;

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
  if (socket.parse_http_headers() < 0)
  {

  }

  std::string str_response("HTTP/1.1 200 OK\r\n\r\n");
  str_response += "<html><body><h1>Server</h1></body></html>";

  if (socket.write(str_response.c_str(), str_response.size()) < 0)
  {

  }
}


