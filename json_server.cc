#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <assert.h>
#include "socket.hh"

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket);

int main(int argc, char *argv[])
{
  tcp_server_t server(2000);
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

void handle_client(socket_t& socket_client)
{
  json_t *response = NULL;
  json_t *request = socket_client.read();

  //get dates
  json_t *json_obj;
  json_obj = json_object_get(request, "start_year");
  json_int_t start_year = json_integer_value(json_obj);
  std::cout << "server received: " << std::endl;
  std::cout << "start_year: " << start_year << std::endl;

  //do response
  response = json_object();
  json_object_set_new(response, "next_year", json_integer(start_year + 1));
  socket_client.write(response);



}


