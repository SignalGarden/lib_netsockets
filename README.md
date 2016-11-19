# lib_netsockets
C++ light wrapper for POSIX and Winsock sockets with implementation of TCP client/server using JSON messages,and HTTP, FTP clients

Dependencies
------------

[Jansson](http://www.digip.org/jansson/)
Jansson is a C library for encoding, decoding and manipulating JSON data.
<br /> 


Building from source
------------

Install dependency packages: Debian-based systems (like Ubuntu)
<pre>
sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install libjansson-dev
</pre>

Install dependency packages: For RPM-based systems (like Fedora and CentOS)
<pre>
sudo yum install cmake
sudo yum install jansson-devel
</pre>

Get source:
<pre>
git clone https://github.com/pedro-vicente/lib_netsockets.git
</pre>

Makefile for most Unix systems
------------
Build and test with:
<pre>
make -f makefile
make -f makefile test
</pre>

Building with CMake in Unix
------------
On most Unix systems, the Jansson library is found on the default location with
<pre>
cd build
cmake ..
</pre>

Building with CMake in Windows
------------
If the Jansson library is not found, it can be set with 
<pre>
cmake .. -DJANSSON_INCLUDE:PATH=/your/jansson/include/path -DJANSSON_LIBRARY=/your/jansson/library/file/name
</pre>

For a Windows Visual Studio build a statically build runtime library can be set with. 
<pre>
cmake .. -DSTATIC_CRT:BOOL=ON
</pre>


#Usage
lib_netsockets is C++ light wrapper for POSIX and Winsock sockets with implementation of TCP client/server using JSON messages,and HTTP, FTP clients.

#TCP server example
```c++
tcp_server_t server(2000);
while (true)
{
 socket_t socket = server.accept_client();
 handle_client(socket);
 socket.close();
}
server.close();
```

#TCP client example
```c++
tcp_client_t client("127.0.0.1", 2000);
client.open();
client.write(buf, strlen(buf));
client.read_some(buf, sizeof(buf));
client.close();
```

#HTTP client example
```c++
http_t client("www.mysite.com", 80);
client.get("/my/path/to/file", true);
```

#FTP client example
Get file list from FTP server and first file in list
```c++
ftp_t ftp("my.ftp.site", 21);
ftp.login("my user", "anonymous");
ftp.get_file_list();
ftp.get_file(ftp.m_file_nslt.at(0).c_str());
ftp.logout();
```

#JSON messages
JSON message requests are made using the jansson JSON library. An example that defines a JSON object defined as 

<pre>
{"start_year": 2016}
</pre>
is

```c++
json_t *request = json_object();
json_object_set_new(request, "start_year", json_integer(2016));
```

The connection and message transmission to the server is made with
```c++
client.open();
client.write(request);
```

The server handles the client request in the function handle_client(), that uses the socket_t class read() function
to parse the JSON message. In this case, the object with JSON key "start_year" is obtained, then a JSON response
is made with and object with key "next_year", that is written to the socket, using the socket_t class write()

```c++
void handle_client(socket_t& socket_client)
{
  json_t *response = NULL;
  json_t *request = socket_client.read();
  json_t *json_obj;
  json_obj = json_object_get(request, "start_year");
  json_int_t start_year = json_integer_value(json_obj);
  response = json_object();
  json_object_set_new(response, "next_year", json_integer(start_year + 1));
  socket_client.write(response);
}
```

The client in turn receives the server response, and closes the opened socket, with

```c++
json_t *response = client.read();
json_t *json_obj;
json_obj = json_object_get(response, "next_year");
json_int_t next_year = json_integer_value(json_obj);
client.close();
```

Test JSON client / server
------------
