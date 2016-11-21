#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <assert.h>
#include "socket.hh"

const unsigned short port = 2001;

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket);

int main(int argc, char *argv[])
{
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

void handle_client(socket_t& socket_client)
{
  json_t *request = socket_client.read();

  //get dates
  json_t *json_obj;
  json_obj = json_object_get(request, "start_year");
  json_int_t start_year = json_integer_value(json_obj);
  std::cout << "server received: " << std::endl;
  std::cout << "start_year: " << start_year << std::endl;

  //do response
  json_t *response = json_object();
  json_t *json = json_object();
  json_object_set_new(response, "next_year", json);

  for (int idx_row = 2; idx_row >= 0; idx_row--)
  {
    std::string str("year_");
    str += std::to_string(static_cast<long long int>(idx_row));
    json_t *arr = json_array();
    json_object_set_new(json, str.c_str(), arr);

    if (json_array_append(arr, json_integer(start_year + 1)) < 0)
    {

    }

    if (json_array_append(arr, json_real(idx_row)) < 0)
    {

    }
  }

  socket_client.write(response);
}


