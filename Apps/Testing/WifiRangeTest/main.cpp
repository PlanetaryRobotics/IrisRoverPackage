// Server side implementation of UDP client-server model  
// https://github.com/cilynx/rtl88x2bu
#include <cstdlib> 
#include <string> 
#include <cstring>

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT   8080 
#define MAXLINE 1024 

using namespace std;

int main() { 
  int sockfd; 
  char buffer[MAXLINE]; 
  string hello = "Hello from server"; 
  struct sockaddr_in servaddr, cliaddr; 
  
  // Creating socket file descriptor 
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  memset(&servaddr, 0, sizeof(servaddr)); 
  memset(&cliaddr, 0, sizeof(cliaddr)); 
  
  // Filling server information 
  servaddr.sin_family = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = INADDR_ANY; 
  servaddr.sin_port = htons(PORT); 
  
  // Bind the socket with the server address 
  if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
      sizeof(servaddr)) < 0 ) 
  { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  int len, n; 

  len = sizeof(cliaddr); //len is value/resuslt 

  n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
        MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
        (socklen_t *)&len); 
  buffer[n] = '\0'; 
  printf("Client : %s\n", buffer); 
  sendto( sockfd, (const char *)hello.c_str(), strlen(hello.c_str()), 
          MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
          len); 
  printf("Hello message sent.\n"); 
  
  return 0; 
} 
