#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>


/*  Lab 2 Description
    1. The client will take one argument, the port on which the server is running
    2. The client will create a DGRAM socket
    3. The client will set the socket to allow broadcast (see code below).
    4. The client will
        a. Set the opcode to 1
        b. Set the fromIPAddr to 0.0.0.0
        c. Set the toIPAddr to 255.255.255.255
        d. Set the ciaddr and yiaddr to 0.0.0.0
        e. Set the transID to a random number
        f. Broadcast the request to the server
    5. The client will then wait for a response, and:
      a. The client will validate the op code is a 2
      b. The server will validate the toIPAddr is 255.255.255.255
    6. If all these conditions are met, the server will
      a. Print all the fields in the packet
*/ 


// data to send request/reply to server
struct dhcp_packet
{
    uint8_t op; // operation code (1 = BOOTREQUEST, 2 = BOOTREPLY)
    uint32_t ciaddr; // client IP address (0 if client is in INIT state)
    uint32_t yiaddr; // 'your' (client) IP address
    uint32_t fromIPAddr; // should be 0.0.0.0 on client->server
    uint32_t toIPAddr; // should be 255.255.255.255 on client->server
    uint16_t transID; // transaction ID – random number client->server
};

// Functions
void createSocket(int* sd);
void broadcast(int sd);
void serverAddress(struct sockaddr_in *sin_addr, int portNumber);
//******   struct dhcp_packet requestPacket();
void broadcastRequest(int sd, struct sockaddr_in *sin_addr);
void receiveResponse(int sd, struct sockaddr_in *sin_addr, struct dhcp_packet *response);
void printResponse(struct dhcp_packet *response);


// Main Function
int main(int argc, char *argv[])
{
    // client takes one argument, port on which server is running
    if (argc < 2)
    {
        printf ("Usage is: client <portNumber>\n");
        exit(EXIT_FAILURE);
    }

    // get portNumber
    int portNumber = atoi(argv[1]);
    
    int sd; /* socket descriptor */
    struct sockaddr_in sin_addr;
    struct dhcp_packet response;

    // create DGRAM socket
    createSocket(&sd);
    // set socket to allow broadcast
    broadcast(sd);
    // add SERVER address
    serverAddress(&sin_addr, portNumber);

    // broadcast request to server (4f)
    broadcastRequest(sd, &sin_addr);
    // receive response from server
    receiveResponse(sd, &sin_addr, &response);


    // validate if op is 2 and toIPAddr is 255.255.255.255 (5a & 5b) 
    // then print all fields in packet (6a)
    if (response.op == 2 && response.toIPAddr == inet_addr("255.255.255.255"))
    {
        printResponse(&response);
    }
    else
    {
        printf("invalid response received.\n");
    }

    // close socket
    close(sd);
    printf("goodbye\n");
    return 0;
}

/* Function definitions */

// create DGRAM socket
void createSocket(int *sd)
{
    if ((*sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("error opening socket");
        exit(1);
    }
}

// set socket to allow broadcast 
void broadcast(int sd)
{
    int broadcast_enable = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
}

// add SERVER address
void serverAddress(struct sockaddr_in *sin_addr, int portNumber)
{
    sin_addr->sin_family = AF_INET;
    sin_addr->sin_port = htons(portNumber);
    sin_addr->sin_addr.s_addr = inet_addr("255.255.255.255");
}

// broadcast request to server (4f)
void broadcastRequest(int sd, struct sockaddr_in *sin_addr)
{
    srand(time(NULL)); // set seed for random generator
    struct dhcp_packet request;
    request.op = 1;
    request.ciaddr = inet_addr("0.0.0.0");
    request.yiaddr = inet_addr("0.0.0.0");
    request.fromIPAddr = inet_addr("0.0.0.0");
    request.toIPAddr = inet_addr("255.255.255.255");
    request.transID = (uint16_t)rand();

    if (sendto(sd, &request, sizeof(request), 0, (struct sockaddr *)sin_addr, sizeof(*sin_addr)) < 0)
    {
        perror("error sending request");
        close(sd);
        exit(1);
    }
}

/* . . . wait for response . . .*/

// receive response from server
void receiveResponse(int sd, struct sockaddr_in *sin_addr, struct dhcp_packet *response)
{   
    socklen_t addr_len = sizeof(sin_addr);
    if (recvfrom(sd, response, sizeof(*response), 0, (struct sockaddr *)sin_addr, &addr_len) < 0)
    {
        perror("error receiving response");
        // close(sd);
        exit(1);
    }
}

// print response packet
void printResponse(struct dhcp_packet *response)
{
    printf("Received valid response:\n");
    printf("Operation code: %d\n", response->op);
    printf("Client IP Address (ciaddr): %s\n", inet_ntoa(*(struct in_addr *)&response->ciaddr));
    printf("Your IP Address (yiaddr): %s\n", inet_ntoa(*(struct in_addr *)&response->yiaddr));
    printf("From IP Address: %s\n", inet_ntoa(*(struct in_addr *)&response->fromIPAddr));
    printf("To IP Address: %s\n", inet_ntoa(*(struct in_addr *)&response->toIPAddr));
    printf("Transaction ID: %u\n", response->transID);
}
