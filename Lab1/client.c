#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>


/*  Lab 1 Desciption
    1.	Take the server’s port number and an ip address as command line arguments
    2.	Connect to the server on the port number and ip address
    3.	Ask the user for an input string (Max size of string is 256 bytes)
    4.	Send the length of the string to the server (remember to convert using htonl())
    5.	Send the string to the server (don’t send more bytes than are in the string)
    6.	Receive back from the server (in network byte order) the number of bytes the server is echoing back. 
        Note it may/will be different than the number of bytes you sent over. 
        Remember to convert this back to host order using ntohl()
    7.	Receive the number of bytes back from the server. 
    8.	Print out the string the server sends back. Make sure it matches what the server says (see video)
    9.	Comment your code. 
    10.	Use structured programming (not everything is in the main function!)
*/    


// Functions
int createSocket();
void connectToServer(int sd, char *ipAddress, int portNumber);
void getInputString(char *inputString);
void sendData(int sd, char *inputString);
void receiveData(int sd);
void closeSocket(int sd);


// Main function
int main(int argc, char *argv[]) 
{
    if (argc < 3) {
        printf("Usage is: Client <portNumber> <ipaddress>\n");
        exit(1);
    }

    // get portNumber
    int portNumber = atoi(argv[1]);
    // get ipaddress 
    char *ipAddress = argv[2];

    // create socket
    int sd = createSocket();  
    // connect to the server
    connectToServer(sd, ipAddress, portNumber);  

    // account for null 
    char inputString[257];  
    // get user input
    getInputString(inputString);  

    // send data to server
    sendData(sd, inputString);  
    // receive data from server
    receiveData(sd);  

    // close socket
    closeSocket(sd);  

    return 0;
}

/* Function definitions */

// create socket 
int createSocket() 
{
    int sd;
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("error opening stream socket");
      exit(1);
    }
}

// connect to server
void connectToServer(int sd, char *ipAddress, int portNumber) 
{
    // add SERVER address 
    struct sockaddr_in sin_addr;
    sin_addr.sin_family = AF_INET;
    sin_addr.sin_port = htons(portNumber);

    // converting ipAddress
    int rc = inet_pton(AF_INET, ipAddress, &(sin_addr.sin_addr));
    if (rc <= 0) {
        printf("failed to convert ip address\n");
        exit(1);
    }

    // connect to server on portNumber and ipAddress
    rc = connect(sd, (struct sockaddr*)&sin_addr, sizeof(struct sockaddr_in));
    if (rc < 0) {
        perror("error connecting stream socket");
        close(sd);
        exit(1);
    }
}

// ask user for input string
void getInputString(char *inputString) 
{
  printf ("Please enter input string (max size: 256 bytes): \n");
  fgets(inputString, 257, stdin);
}

// send string length and string itself to server
void sendData(int sd, char *inputString) 
{
  // length of inputString 
  int stringLength = strlen(inputString);
  int convertedLength = htonl(stringLength);

  // send length of inputString to server
  int rc = write(sd, &convertedLength, sizeof(int));
  if (rc < sizeof(int)) 
    {
      printf("error sending string length\n");
      close(sd);
      exit(1);
    }

  // send inputString itself
  rc = write(sd, inputString, stringLength);
  if (rc < stringLength) 
    {
      printf("error sending input string\n");
      close(sd);
      exit(1);
    }
}

// receive back from server (network byte order) number of bytes server echoing back
// note: may/will be different than number of bytes you sent over
void receiveData(int sd) 
{
  int echoedLength;
  int rc = read(sd, &echoedLength, sizeof(int));
  if (rc < sizeof(int)) 
    {
      printf("Error receiving echoed string length\n");
      close(sd);
      exit(1);
    }

  // convert back to host order using ntohl()
  echoedLength = ntohl(echoedLength);
  // receive back number of bytes echoed by server
  printf("Server echoed back %d bytes\n", echoedLength);

  // print out string server sends back. Make sure matches what server says (see video)
  char echoedString[echoedLength + 1];   // account for null 
  rc = read(sd, echoedString, echoedLength);
  if (rc < echoedLength)
    {
      printf("Error receiving echoed string\n");
      close(sd);
      exit(1);
    }

  echoedString[echoedLength] = '\0';     // account for null
  printf("Server echoed back the string: %s\n", echoedString);
}

// Closes the socket
void closeSocket(int sd) 
{
    close(sd);
    printf("goodbye\n");
}