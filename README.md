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


# usage
lib_netsockets is C++ light wrapper for POSIX and Winsock sockets with implementation of TCP client/server using JSON messages,and HTTP, FTP clients.

#TCP server example
<pre>
tcp_server_t server(2000);
while (true)
{
 socket_t socket = server.accept_client();
 handle_client(socket);
 socket.close();
}
server.close();
</pre>

#TCP client example
<pre>
tcp_client_t client("127.0.0.1", 2000);
client.open();
client.write(buf, strlen(buf));
client.read_some(buf, sizeof(buf));
client.close();
</pre>

#HTTP client example
<pre>
http_t client("www.mysite.com", 80);
client.get("/my/path/to/file", true);
</pre>

#FTP client example
Get file list from FTP server and first file in list
<pre>
ftp_t ftp("my.ftp.site", 21);
ftp.login("my user", "anonymous");
ftp.get_file_list();
ftp.get_file(ftp.m_file_nslt.at(0).c_str());
ftp.logout();
</pre>



