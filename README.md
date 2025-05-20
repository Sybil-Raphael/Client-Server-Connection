# Client-Server Connections

This repository contains two client-server networking labs.

## Lab 1: TCP Client

### Description

This lab implements a TCP client that communicates with a server. The client performs the following actions:

1.  Takes the server's port number and IP address as command-line arguments.
2.  Connects to the server on the specified port and IP address.
3.  Prompts the user for an input string (with a maximum size of 256 bytes).
4.  Sends the length of the string to the server (after converting it to network byte order using `htonl()`).
5.  Sends the string to the server.
6.  Receives the number of bytes the server is echoing back (in network byte order). This number may be different from the number of bytes sent. The client converts this value to host byte order using `ntohl()`.
7.  Receives the echoed data from the server.
8.  Prints the string received from the server.
9.  The code is commented for clarity and follows structured programming principles.

### How to Run

1.  Compile the `client.c` code (e.g., using `gcc client.c -o client`).
2.  Run the client executable from the command line, providing the server's IP address and port number:

    ```bash
    ./client <ipAddress> <portNumber>
    ```

## Lab 2: UDP Client

### Description

This lab implements a UDP client that broadcasts a request to a server. The client performs the following actions:

1.  Takes the port on which the server is running as a command-line argument.
2.  Creates a DGRAM (UDP) socket.
3.  Sets the socket to allow broadcasting.
4.  Constructs a request packet with the following fields:
    * Opcode set to 1.
    * `fromIPAddr` set to 0.0.0.0.
    * `toIPAddr` set to 255.255.255.255.
    * `ciaddr` and `yiaddr` set to 0.0.0.0.
    * `transID` set to a random number.
5.  Broadcasts the request packet to the server.
6.  Waits for a response from the server.
7.  Validates the response packet:
    * Checks that the opcode is 2.
    * Checks that the `toIPAddr` is 255.255.255.255.
8.  If the response is valid, the client prints all the fields in the received packet.

### How to Run

1.  Compile the `DGclient.c` code (e.g., using `gcc DGclient.c -o DGclient`).
2.  Run the client executable from the command line, providing the server's port number:

    ```bash
    ./DGclient <portNumber>
    ```
