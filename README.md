# Socket-transfer-client-server
Transfer data between client and server using a hostname and port number implemented with sockets in C.
usage:

step 1:
```
make
```
step2:
```
Client:
./transferclient -p <port no.> -s <servername/hostname> -o <file to receive>

Server:
./transferserver -f <file to serve> -p <port no.>
```
# Project Design
In this project, the server sends the content of a large file to the connected client.  
<image src="https://raw.githubusercontent.com/sreeganeshji/socket-echo-client-server/master/Illustrations/Transfer.png" height=150>
# Trade offs considered 
Since the received message size is unknown, a cumulative approach to receiving the chunks of message and writing them into an output file was taken. 
# Flow of control
The server takes the port number and file name as inputs. It binds a socket to the port number, it opens the file to be send and waits for any connections form the client.  
The client creates a socket to connect to the server and a file under the requested filename. Once the connection is established, the server transfers the content of the file and the client receives it and writes to the file repeatedly till all the contents of the file have been transferred. 
# Code implementation
The server creates and binds the socket using the address obtained by getaddinfo method. It opens the file requested by the filename using the method fopen. It sets the pointer to the file to 0 using fseek. It then obtains the content using fgets using a buffer. It then connects to the requesting client and sends the buffer. 
# Code testing
The code was tested to work with different port numbers, different length files, invalid connection parameters, and so on. 
# References
The main reference that was used for networking aspects was Beej’s Guide to programming pg 29-30. The aspects of file transfer were referred to on https://fresh2refresh.com/ 
