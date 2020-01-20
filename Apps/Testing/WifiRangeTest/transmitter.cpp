// Server side implementation of UDP client-server model  
// https://github.com/cilynx/rtl88x2bu
#include <cstdlib> 
#include <string> 
#include <cstring>
#include <iostream>

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT              8080
#define CLIENT_IP_ADDRESS "192.168.1.2"
#define TIMEOUT_RX_SECOND 1

using namespace std;

int main(int argc, char *argv[]){

  int sockfd; 
  int result; 
  struct sockaddr_in servaddr, cliaddr; 
  string devname = "wlx00e04c295d5e";
  char *txBuffer;
  char *rxBuffer; 
  uint32_t nbOfMessageToSend;
  uint32_t txFrequency;
  uint32_t payloadSize;
  struct timeval tv;
  uint32_t receivedBytes;

  if(argc < 4){
    cout << "Usage Transmitter <nbOfMessageToSend> <txFrequency> <payloadSize>" << endl;
    exit(EXIT_FAILURE);
  }

  if(atoi(argv[2]) == 0){
    cout << "Frequency cannot equal to 0!" << endl;
    exit(EXIT_FAILURE);
  }

  if(atoi(argv[3]) < 1){
    cout << "Payload size must be at least equal to 1" << endl;
    exit(EXIT_FAILURE);
  }

  nbOfMessageToSend = atoi(argv[1]);
  txFrequency = atoi(argv[2]);
  payloadSize = atoi(argv[3]);

  txBuffer = (char *)malloc(payloadSize);
  rxBuffer = (char *)malloc(payloadSize);

  // Creating socket file descriptor 
  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  setsockopt(sockfd, IPPROTO_UDP, SO_BINDTODEVICE, devname.c_str(), devname.length());
  
  // set the timeout for the reception
  tv.tv_sec = TIMEOUT_RX_SECOND;
  tv.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  memset(&servaddr, 0, sizeof(servaddr)); 
  memset(&cliaddr, 0, sizeof(cliaddr)); 
  
  // Server information 
  servaddr.sin_family = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(PORT);

  // Client address information
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_addr.s_addr = inet_addr(CLIENT_IP_ADDRESS);
  cliaddr.sin_port = htons(PORT); 
  
  // Bind the socket with the server address 
  if(bind(sockfd, 
          (const struct sockaddr *)&servaddr, 
          sizeof(servaddr)) < 0 ){ 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  for(uint32_t i=0; i<nbOfMessageToSend; i++){
    // First 4 bytes indicates the packet number
    memcpy(txBuffer, &i, sizeof(i));

    // Fill buffer with some test bytes
    for(uint32_t ii=0; ii<payloadSize; ii++){
      memset(txBuffer+sizeof(i), 0xAA, sizeof(txBuffer) - sizeof(i));
    }

    result = sendto(sockfd,
                    txBuffer,
                    sizeof(txBuffer), 
                    0,
                    (struct sockaddr *) &cliaddr, 
                    sizeof(cliaddr));

    cout << "Tx[" << i+1 << "/" << nbOfMessageToSend << "] ";

    if(result < 0){
      cout << "Error during data sent:" << strerror(errno) << endl;
      exit(EXIT_FAILURE);
    }
    else{
      cout << endl;
    }


    receivedBytes = sizeof(rxBuffer);

    result = recvfrom(sockfd,
                      rxBuffer,
                      sizeof(rxBuffer),
                      0,
                      (struct sockaddr *) &cliaddr,
                      &receivedBytes);

    cout << "Rx[" << i+1 << "/" << nbOfMessageToSend << "] ";

    if(result < 0){
      cout << "Error[" << errno << "] during reception: " << strerror(errno) << endl;
    }

    usleep(1000000 / txFrequency); 
  } 

  cout << "Done!" << endl;
  
  return 0; 
} 
