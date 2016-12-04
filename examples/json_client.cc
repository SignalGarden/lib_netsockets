#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <jansson.h>
#include "socket.hh"

const unsigned short port = 2001;

///////////////////////////////////////////////////////////////////////////////////////
//main
//TCP client that writes and reads JSON messages 
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  char buf_server[255]; // server host name or IP
  char response_name[255]; // add to response.json
  char* buf = NULL;
  std::string str_response;

  if (argc < 3)
  {
    std::cout << "usage: ./json_client -s <star server IP address/host name>" << std::endl;
    return 1;
  }

  for (int i = 1; i < argc && argv[i][0] == '-'; i++)
  {
    switch (argv[i][1])
    {
    case 's':
      strcpy(buf_server, argv[i + 1]);
      i++;
      break;
    case 'f':
      strcpy(response_name, argv[i + 1]);
      str_response = response_name;
      i++;
      break;
    }
  }


  //make JSON
  json_t *request = json_object();
  json_object_set_new(request, "start_year", json_integer(2016));

  json_dump_file(request, "request.json", 0);

  tcp_client_t client(buf_server, port);
  std::cout << "client connecting to: " << buf_server << ":" << port << std::endl;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //create socket and open connection
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.open();
  std::cout << "client connected to: " << buf_server << ":" << port << std::endl;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //write request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.write(request);
  std::cout << "client sent: ";

  buf = json_dumps(request, 0);
  std::cout << buf << std::endl;
  free(buf);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //read response
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  json_t *response = client.read();

  std::string str_response_name("response");
  if (str_response.size())
  {
     str_response_name += str_response;
  }
  str_response_name += ".json";
  json_dump_file(response, str_response_name.c_str(), 0);

  std::cout << "client received: ";

  buf = json_dumps(response, 0);
  std::cout << buf << std::endl;
  free(buf);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //close connection
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.close();

  return 0;
}



