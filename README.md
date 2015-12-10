Language: C
Project: TCP and UDP sockets

Instructions:
change into the directory containing the files

- TCP

server (the sender)
compile with:  gcc tcp_server.cc -o tcp_server
run with: ./tcp_server

client (recipient of data)
compile with: gcc tcp_client.cc -o tcp_client
run with: ./tcp_client 127.0.0.1 (or your host name)

- UDP

server (recipient of data)
compile with:  gcc udp_server.cc -o udp_server
run with: ./udp_server

client (the sender)
compile with: gcc udp_client.cc -o udp_client
run with: ./udp_client 127.0.0.1 (or your host name)

Outcome:
A doubling amount of bytes (starting at 1 byte) wil be sent in a TCP/UDP packet from the server to the client.
When each packet is sent/received, printed will be the current time in seconds.
This is used to measure the throughput of the local machine and compare TCP v UDP performance. This information will be saved to a local text file.
