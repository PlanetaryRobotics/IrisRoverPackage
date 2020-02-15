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
#define MAX_TX_BUFFER_SIZE    10
#define MAX_RX_BUFFER_SIZE    10

#define MOTOR_GEARBOX_RATIO     416
#define MOTOR_TICKS_PER_MECH_REV  24      
#define ROVER_MOTOR_MAX_SPEED_RPM 8000      

using namespace std;


/**
 * @brief      main function
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     Exit code
 */
int main(int argc, char *argv[]){
  // Network variables
  int sockfd; 
  int result; 
  struct sockaddr_in servaddr, cliaddr; 
  string devname = WIRELESS_ADAPTER_NAME;
  char txBuffer[MAX_TX_BUFFER_SIZE];
  char rxBuffer[MAX_RX_BUFFER_SIZE]; 
  struct timeval tv;
  uint32_t bytesToRead;

  // Logging variables
  ofstream logFile;
  string testDescription;
  
  // Rover related variable
  float targetPosRevLeft;     // revolution
  float targetSpeedRevLeft;   // rev/s
  float targetPosRevRight;    // revolution
  float targetSpeedRevRight;  // rev/s
                                                
  // Create log file
  time_t now = time(0);
  string nowDate = ctime(&now);
  logFile.open(nowDate.c_str());

  // check integrity of arguments
  if(argc < 5){
    cout << "Usage: Mobility <left speed (rev/min)>  <relative target position left (rev)> <right speed (rev/min )> <relative target position right (rev)>" << endl;
    exit(EXIT_FAILURE);
  }

  // Create simple description in the log file
  // cout << "Enter short description of test:" << endl;
  // cin >> testDescription;
  // logFile << testDescription.c_str();
  // logFile << endl;

  // Capture and convert commands to send to the rover
  targetSpeedRevLeft = atof(argv[1]);
  targetPosRevLeft = atoi(argv[2]);
  targetSpeedRevRight = atof(argv[3]);
  targetPosRevRight = atoi(argv[4]);

  logFile << "Target speed left (rev/min): " << targetSpeedRevLeft << endl;
  logFile << "Target speed right (rev/min): " << targetSpeedRevRight << endl;

  logFile << "Target pos left (rev): " << targetPosRevLeft << endl;
  logFile << "Target pos right (rev): " << targetPosRevRight << endl;

  // translate commands to motor control format
  float roverMaxRevSpeed = (float)(ROVER_MOTOR_MAX_SPEED_RPM) / (float)(MOTOR_GEARBOX_RATIO); 
  float targetSpeedRevPercentLeft = (uint16_t) ( (float) targetSpeedRevLeft / (float) roverMaxRevSpeed * 100.0);
  float targetSpeedRevPercentRight = (uint16_t) ( (float) targetSpeedRevRight / (float) roverMaxRevSpeed * 100.0);

  int32_t targetPosTicksLeft =  targetPosRevLeft * MOTOR_GEARBOX_RATIO * MOTOR_TICKS_PER_MECH_REV;
  int32_t targetPosTicksRight =  targetPosRevRight * MOTOR_GEARBOX_RATIO * MOTOR_TICKS_PER_MECH_REV;

  cout << "Rover theorical max speed (rev/min): " << roverMaxRevSpeed << endl;
  cout << "Target speed left (rev/min): " << targetSpeedRevLeft << " (" << targetSpeedRevPercentLeft << "%)" << endl;
  cout << "Target position left (rev): " << targetPosRevLeft << " (" << targetPosTicksLeft << " ticks)" << endl;
  cout << "Target speed right (rev/min): " << targetSpeedRevRight << " (" << targetSpeedRevPercentRight << "%)" << endl;
  cout << "Target position right (rev): " << targetPosRevRight << " (" << targetPosTicksRight << " ticks)" << endl;

  targetSpeedRevPercentLeft = (float)(targetSpeedRevPercentLeft) * 0.63; // scale to motor control command
  targetSpeedRevPercentRight = (float)(targetSpeedRevPercentRight) * 0.63; // scale to motor control command

  // Prepare transmission buffer
  txBuffer[0] = targetSpeedRevPercentLeft;
  txBuffer[1] = targetSpeedRevPercentRight;

  memcpy(txBuffer+2,
         &targetPosTicksLeft,
         sizeof(targetPosTicksLeft));

  memcpy(txBuffer+2+sizeof(targetPosTicksLeft),
         &targetPosTicksRight,
         sizeof(targetPosTicksRight));

  // Creating socket file descriptor 
  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  // Configure network socket
  setsockopt(sockfd, IPPROTO_UDP, SO_BINDTODEVICE, devname.c_str(), devname.length());
  
  // Set the timeout for the reception socket
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

  // send data over UDP
  result = sendto(sockfd,
                  txBuffer,
                  sizeof(txBuffer), 
                  0,
                  (struct sockaddr *) &cliaddr, 
                  sizeof(cliaddr));

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

    if(result >= 0){
      bytesToRead -= result;
    }
  }

  return 0; 
} 
