# lib_netsockets
C++ light wrapper for POSIX and Winsock sockets

Get source:
<pre>
git clone https://github.com/pedro-vicente/lib_netsockets.git
</pre>

Build with:
Unix
<pre>
make -f makefile
</pre>
Windows, Visual Studio project in ./build

# usage
lib_netsockets is a light wrapper for POSIX and Winsock sockets.

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



