#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <assert.h>
#include <time.h>
#include <ctime>
#include "socket.hh"

const unsigned short port = 2001;

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
//main
///////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket);

int main(int argc, char *argv[])
{
  tcp_server_t server(port);
  std::cout << "server listening on port " << port << std::endl;
  while (true)
  {
    char *str_ip;
    char* buf;
    json_t *response = NULL;
    json_t *request = NULL;
    socket_t socket = server.accept_client();

    // convert IP addresses from a dots-and-number string to a struct in_addr and back
    str_ip = inet_ntoa(socket.m_sockaddr_in.sin_addr);
    std::cout << prt_time() << "server accepted: " << str_ip << "," << socket.m_socket_fd << std::endl;

    if ((request = socket.read()) != NULL)
    {
      buf = json_dumps(request, JSON_PRESERVE_ORDER & JSON_COMPACT);
      std::cout << prt_time() << "server received: " << str_ip << "," << socket.m_socket_fd << " " << buf << std::endl;
      free(buf);
    }


    //get dates
    json_t *json_obj;
    json_obj = json_object_get(request, "start_year");
    json_int_t start_year = json_integer_value(json_obj);

    //do response
    response = json_object();
    json_object_set_new(response, "next_year", json_integer(start_year + 1));

    if (socket.write(response) > 0)
    {
      buf = json_dumps(response, JSON_PRESERVE_ORDER & JSON_COMPACT);
      std::cout << prt_time() << "server sent to: " << str_ip << "," << socket.m_socket_fd << " " << buf << std::endl;
      free(buf);
    }

    socket.close();
    json_decref(request);
    json_decref(response);
  }
  server.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//handle_client
/////////////////////////////////////////////////////////////////////////////////////////////////////

void handle_client(socket_t& socket)
{
  json_t *request = socket.read();

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

    if (json_array_append_new(arr, json_integer(start_year + 1)) < 0)
    {

    }

    if (json_array_append_new(arr, json_real(idx_row)) < 0)
    {

    }
  }

  socket.write(response);
}


