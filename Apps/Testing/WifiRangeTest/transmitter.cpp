#include <cstdlib> 
#include <string> 
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define ROVER_PORT              8080
#define LOCAL_PORT              8080
#define CLIENT_IP_ADDRESS       "192.168.1.2"
#define TIMEOUT_RX_SECOND       10
#define WIRELESS_ADAPTER_NAME   "wlx00e04c295d5e"

using namespace std;


int main(int argc, char *argv[]){
  int sockfd; 
  int result; 
  struct sockaddr_in servaddr, cliaddr; 
  string devname = WIRELESS_ADAPTER_NAME;
  char *txBuffer;
  char *rxBuffer; 
  uint32_t nbOfMessageToSend;
  uint32_t txFrequency;
  uint32_t payloadSize;
  struct timeval tv;
  uint32_t receivedBytes;
  std::vector<int> timeoutLogger;
  std::vector<int> dataErrorLogger;
  uint32_t bytesToRead;
  ofstream logFile;
  string testDescription;
  float rssiTotal = 0;
  float snrTotal = 0;
  uint32_t averageSignalSample = 0;
  float averageRssi = 0;
  float averageSnr = 0;
  bool badDataFlag = false;

  time_t now = time(0);
  string nowDate = ctime(&now);
  logFile.open(nowDate.c_str());

  // check integrity of arguments
  if(argc < 4){
    cout << "Usage Transmitter <nbOfMessageToSend> <txFrequency> <payloadSize>" << endl;
    exit(EXIT_FAILURE);
  }

  if(atoi(argv[2]) == 0){
    cout << "Frequency cannot equal to 0!" << endl;
    exit(EXIT_FAILURE);
  }

  if(atoi(argv[3]) < 8 || atoi(argv[3]) > 2038){
    cout << "Payload size must be between 8 and 2038" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Enter short description of test:" << endl;
  cin >> testDescription;
  logFile << testDescription.c_str();
  logFile << endl;

  nbOfMessageToSend = atoi(argv[1]);
  txFrequency = atoi(argv[2]);
  payloadSize = atoi(argv[3]);

  txBuffer = (char *)malloc(payloadSize*sizeof(char));
  rxBuffer = (char *)malloc(payloadSize*sizeof(char));

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
  servaddr.sin_port = htons(LOCAL_PORT);

  // Client address information
  cliaddr.sin_family = AF_INET;
  cliaddr.sin_addr.s_addr = inet_addr(CLIENT_IP_ADDRESS);
  cliaddr.sin_port = htons(ROVER_PORT); 
  
  // Bind the socket with the server address 
  if(bind(sockfd, 
          (const struct sockaddr *)&servaddr, 
          sizeof(servaddr)) < 0 ){ 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  // Initialize transmission buffer
  for(uint32_t i=0; i<payloadSize; i++){
    txBuffer[i] = 0xAA;
  }

  // Enter main testing loop
  for(uint32_t i=0; i<nbOfMessageToSend; i++){
    
    badDataFlag = false;

    // First 4 bytes indicates the packet number
    memcpy(txBuffer, &i, sizeof(i));
    
    // Following byte
    memcpy(txBuffer+sizeof(i), &payloadSize, sizeof(payloadSize));

    // send data over UDP
    result = sendto(sockfd,
                    txBuffer,
                    payloadSize, 
                    0,
                    (struct sockaddr *) &cliaddr, 
                    sizeof(cliaddr));

    // Print transmission acknolewdgement to user 
    cout << "Tx[" << i+1 << "/" << nbOfMessageToSend << "] ";

    // Should not fail here
    if(result < 0){
      cout << "Error during data sent:" << strerror(errno) << endl;
      break;
    }

    bytesToRead = payloadSize;

    // Loop until all data is received. The socket is configured to timeout
    // and will return -1 on that event. Timeouts are logged into vector to
    // trace which packet failed
    while(bytesToRead > 0){
      socklen_t addrlen = sizeof(cliaddr);
      result = recvfrom(sockfd,
                        rxBuffer,
                        bytesToRead, /* max size of udp packet from wf121 */
                        0,
                        (struct sockaddr *) &cliaddr,
                        &addrlen);

      if(result < 0){
        cout << "Error[" << errno << "] during reception: " << strerror(errno) << endl;
        timeoutLogger.push_back(i+1);
        break;
      }
      else{
        bytesToRead -= result;
      }
    }

    cout << "Rx[" << i+1 << "/" << nbOfMessageToSend << "] ";
    

    // check data consistency
    for(uint32_t ii=10; ii<payloadSize; ii++){
      if(txBuffer[ii] != rxBuffer[ii]){
        dataErrorLogger.push_back(i+1);
        badDataFlag = true;
        break;
      }
    }

    //do the average of the rssi and snr
    if(!badDataFlag){
      cout << " RSSI: " << (int)(rxBuffer[8]) << " SNR: " << (int)(rxBuffer[9]);
      rssiTotal += rxBuffer[8]; 
      snrTotal += rxBuffer[9];
      averageSignalSample++;
    }

    cout << endl;

    // Throttle the loop frequency
    usleep(1000000 / txFrequency); 
  } 

  averageRssi = rssiTotal / (float)averageSignalSample;
  averageSnr = snrTotal / (float)averageSignalSample; 

  cout << "--------------------------------------------" << endl;
  cout << "Done!" << endl;

  logFile << "Summary:" << endl;
  logFile << "Message send:" << nbOfMessageToSend << endl;
  logFile << "Number of bytes sent: " << payloadSize * nbOfMessageToSend << endl;
  logFile << "Number of timeout error: " << timeoutLogger.size() << " (" << (float)(timeoutLogger.size()) / (float)(nbOfMessageToSend) * 100.0 << " %)" << endl;
  logFile << "Number of data integrity error: " << dataErrorLogger.size() << " (" << (float)(dataErrorLogger.size()) / (float)(nbOfMessageToSend) * 100.0 << " %)" << endl;
  logFile << "Average RSSI:" << averageRssi << endl;
  logFile << "Wifi network average SNR: " << averageSnr << endl;

  for(uint32_t i=0; i<timeoutLogger.size(); i++){
    logFile << "Timeout error on packet # " << timeoutLogger.at(i) << endl;
  }

  for(uint32_t i=0; i<dataErrorLogger.size(); i++){
    logFile << "Data integrity error on packet #" << dataErrorLogger.at(i) << endl;
  }
  
  return 0; 
} 
