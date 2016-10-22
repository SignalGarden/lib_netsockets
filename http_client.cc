#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "http.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//usage
/////////////////////////////////////////////////////////////////////////////////////////////////////

void usage()
{
  std::cout << "usage: http_client -s SERVER -f /FILE -v" << std::endl;
  std::cout << "-s SERVER: fully qualified web server name" << std::endl;
  std::cout << "-f FILE: file located at web server root; file path name must start with '/'" << std::endl;
  std::cout << "-v: verbose, output of retrieved file is printed" << std::endl;
  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////
//main
//HTTP client 
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  const char *host_name = NULL; // server name 
  const char *path_name = NULL; // name of file to retrieve
  bool verbose = false;

  if (argc < 5)
  {
    usage();
  }

  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 'v':
        verbose = true;
        break;
      case 's':
        host_name = argv[i + 1];
        i++;
        break;
      case 'f':
        path_name = argv[i + 1];
        i++;
        break;
      default:
        usage();
      }
    }
    else
    {
      usage();
    }
  }

  http_t client(host_name, 80);
  client.get(path_name, verbose);
  return 0;
}

