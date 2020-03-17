#include <cstdlib> 
#include <string> 
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>
#include <signal.h>

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define ROVER_PORT              8080
#define LOCAL_PORT              8080
#define CLIENT_IP_ADDRESS       "192.168.1.2"
#define TIMEOUT_RX_SECOND       1
#define WIRELESS_ADAPTER_NAME   "wlx00e04c295d5e"
#define MAX_TX_BUFFER_SIZE    10
#define MAX_RX_BUFFER_SIZE    10

#define MOTOR_GEARBOX_RATIO     416
#define MOTOR_TICKS_PER_MECH_REV  24      
#define ROVER_MOTOR_MAX_SPEED_RPM 8000  
#define MAX_MEASURABLE_CURRENT 0.7625	// A    

using namespace std;
using namespace chrono;

uint8_t g_rxBuffer[MAX_RX_BUFFER_SIZE]; 
uint8_t g_txBuffer[MAX_TX_BUFFER_SIZE];
// Network variables
int g_sockfd; 
struct sockaddr_in g_servaddr, g_cliaddr; 
bool g_runApp;

typedef enum CommandList{
  SPEED_MOTOR_LEFT =      0x00,
  SPEED_MOTOR_RIGHT =     0x01,
  POSITION_MOTOR_LEFT =   0x02,
  POSITION_MOTOR_RIGHT =  0x03,
  RUN =                   0x04,
  STOP =                  0x05,
  GET_CURRENT =           0x06,
  GET_POSITION =          0x07,
  GET_STATUS =            0x08
}CommandList;

typedef enum MotorStatus{
  REACHING_TARGET = 0x01,
  TARGET_REACHED = 0x02
}MotorStatus;

void sendCmd(const uint8_t size){
  int result; 
  // send data over UDP
  result = sendto(g_sockfd,
                  g_txBuffer,
                  size, 
                  0,
                  (struct sockaddr *) &g_cliaddr, 
                  sizeof(g_cliaddr));
}

void readData(const uint8_t size){
  uint8_t bytesToRead = size;
  int result;

  // Loop until all data is received. The socket is configured to timeout
  // and will return -1 on that event. Timeouts are logged into vector to
  // trace which packet failed
  while(bytesToRead > 0){
    socklen_t addrlen = sizeof(g_cliaddr);
    result = recvfrom(g_sockfd,
                      g_rxBuffer,
                      bytesToRead, /* max size of udp packet from wf121 */
                      0,
                      (struct sockaddr *) &g_cliaddr,
                      &addrlen);

    if(result >= 0){
      bytesToRead -= result;
    }

    if(result < 0){
      cout << "Timeout during data reception..." << endl;
      break;
    }
  }
}

void setTargetPositionLeft(const int32_t position){
  uint8_t cmdSize = 5;
  g_txBuffer[0] = CommandList::POSITION_MOTOR_LEFT;
  memcpy(g_txBuffer+1, &position, sizeof(position));
  sendCmd(cmdSize);
}

void setTargetPositionRight(const int32_t position){
  uint8_t cmdSize = 5;
  g_txBuffer[0] = CommandList::POSITION_MOTOR_RIGHT;
  memcpy(g_txBuffer+1, &position, sizeof(position));
  sendCmd(cmdSize);
}

void setTargetSpeedLeft(const int16_t speedPercentFront, const int16_t speedPercentBack){
  uint8_t cmdSize = 5;
  g_txBuffer[0] = CommandList::SPEED_MOTOR_LEFT;
  memcpy(g_txBuffer+1, &speedPercentFront, sizeof(speedPercentFront));
  memcpy(g_txBuffer+3, &speedPercentBack, sizeof(speedPercentBack));
  sendCmd(cmdSize);
}

void setTargetSpeedRight(const int16_t speedPercentFront, const int16_t speedPercentBack){
  uint8_t cmdSize = 5;
  g_txBuffer[0] = CommandList::SPEED_MOTOR_RIGHT;
  memcpy(g_txBuffer+1, &speedPercentFront, sizeof(speedPercentFront));
  memcpy(g_txBuffer+3, &speedPercentBack, sizeof(speedPercentBack));
  sendCmd(cmdSize);  
}

void run(){
  uint8_t cmdSize = 1;
  g_txBuffer[0] = CommandList::RUN;
  sendCmd(cmdSize);
}

void stop(){
  uint8_t cmdSize = 1;
  g_txBuffer[0] = CommandList::STOP;
  sendCmd(cmdSize);
}

void getCurrents(int32_t *frontLeft, int32_t *frontRight, int32_t *rearLeft, int32_t *rearRight){
  uint8_t cmdSize = 1;
  uint8_t replySize = 1 /* header */ + 16;
  g_txBuffer[0] = CommandList::GET_CURRENT;

  sendCmd(cmdSize);
  readData(replySize);
  
  if(g_rxBuffer[0] == CommandList::GET_CURRENT){
    memcpy(frontLeft,
           g_rxBuffer + 1,
           sizeof(int32_t));
    memcpy(rearLeft,
           g_rxBuffer + 5,
           sizeof(int32_t));
    memcpy(frontRight,
           g_rxBuffer + 9,
           sizeof(int32_t));
    memcpy(rearRight,
           g_rxBuffer + 13,
           sizeof(int32_t));                     
  }
}

void getPositions(int32_t *frontLeft, int32_t *frontRight, int32_t *rearLeft, int32_t *rearRight){
  uint8_t cmdSize = 1;
  uint8_t replySize = 1 /* header */ + 16;
  g_txBuffer[0] = CommandList::GET_POSITION;

  sendCmd(cmdSize);
  readData(replySize);
  
  if(g_rxBuffer[0] == CommandList::GET_POSITION){
    memcpy(frontLeft,
           g_rxBuffer + 1,
           sizeof(int32_t));
    memcpy(rearLeft,
           g_rxBuffer + 5,
           sizeof(int32_t));
    memcpy(frontRight,
           g_rxBuffer + 9,
           sizeof(int32_t));
    memcpy(rearRight,
           g_rxBuffer + 13,
           sizeof(int32_t));                     
  }
}

uint8_t getStatus(){
  uint8_t cmdSize = 1;
  uint8_t replySize = 1;
  g_txBuffer[0] = CommandList::GET_STATUS;

  sendCmd(cmdSize);
  readData(replySize);

  return g_rxBuffer[0];
}

#define IQ		15

double fixedPointToDouble(int32_t in){
	return (double)in / (double)(1 << IQ);
}

void quitApplication(int s){
  g_runApp = false;
}

std::string dateTime(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,80,"%d-%m-%Y %H-%M-%S", timeinfo);
  return std::string(buffer);
}

/**
 * @brief      main function
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     Exit code
 */
int main(int argc, char *argv[]){
  string devname = WIRELESS_ADAPTER_NAME;
  struct timeval tv;
  uint32_t bytesToRead;
  struct sigaction sigIntHandler;

  // Logging variables
  ofstream logFile;
  string testDescription;

  
  // Rover related variable
  float targetPosRevLeft = 0;     // revolution
  float targetSpeedRevLeftFront = 0;   // rev/s
  float targetSpeedRevLeftBack = 0;
  float targetPosRevRight = 0;    // revolution
  float targetSpeedRevRightFront = 0;  // rev/s
  float targetSpeedRevRightBack = 0;
  
  int32_t positionFrontRight = 0;
  int32_t positionFrontLeft = 0;
  int32_t positionRearRight = 0;
  int32_t positionRearLeft = 0;

  int32_t currentFrontRight = 0;
  int32_t currentFrontLeft = 0;
  int32_t currentRearRight = 0;
  int32_t currentRearLeft = 0;

  string filename = dateTime() + ".txt";
  logFile.open(filename);

  sigIntHandler.sa_handler = quitApplication;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  g_runApp = true;

  // check integrity of arguments
  if(argc < 7){
    cout << "Usage: Mobility <left speed front (rev/min)>  <left speed back (rev/min)> <relative target position left (rev)> <right speed front (rev/min)> <right speed back (rev/min)> <relative target position right (rev)>" << endl;
    exit(EXIT_FAILURE);
  }

  // Create simple description in the log file
  // cout << "Enter short description of test:" << endl;
  // cin >> testDescription;
  // logFile << testDescription.c_str();
  // logFile << endl;

  // Capture and convert commands to send to the rover
  targetSpeedRevLeftFront = atof(argv[1]);
  targetSpeedRevLeftBack= atof(argv[2]);
  targetPosRevLeft = atoi(argv[3]);
  targetSpeedRevRightFront = atof(argv[4]);
  targetSpeedRevRightBack = atof(argv[5]);
  targetPosRevRight = atoi(argv[6]);

  logFile << "Target speed left front (rev/min): " << targetSpeedRevLeftFront << endl;
  logFile << "Target speed left back (rev/min): " << targetSpeedRevLeftBack << endl;
  logFile << "Target speed right front (rev/min): " << targetSpeedRevRightFront << endl;
  logFile << "Target speed right back (rev/min): " << targetSpeedRevRightBack << endl;

  logFile << "Target pos left (rev): " << targetPosRevLeft << endl;
  logFile << "Target pos right (rev): " << targetPosRevRight << endl;

  // translate commands to motor control format
  float roverMaxRevSpeed = (float)(ROVER_MOTOR_MAX_SPEED_RPM) / (float)(MOTOR_GEARBOX_RATIO); 
  int16_t targetSpeedRevPercentLeftFront = (int16_t) ( (float) targetSpeedRevLeftFront / (float) roverMaxRevSpeed * 127.0);
  int16_t targetSpeedRevPercentRightFront = (int16_t) ( (float) targetSpeedRevRightFront / (float) roverMaxRevSpeed * 127.0);
  int16_t targetSpeedRevPercentLeftBack = (int16_t) ( (float) targetSpeedRevLeftBack / (float) roverMaxRevSpeed * 127.0);
  int16_t targetSpeedRevPercentRightBack = (int16_t) ( (float) targetSpeedRevRightBack / (float) roverMaxRevSpeed * 127.0);

  if(targetSpeedRevPercentLeftFront > 127) targetSpeedRevPercentLeftFront = 127;
  if(targetSpeedRevPercentRightFront > 127) targetSpeedRevPercentRightFront = 127;
  if(targetSpeedRevPercentRightFront < -128) targetSpeedRevPercentRightFront = -128;
  if(targetSpeedRevPercentLeftFront < -128) targetSpeedRevPercentLeftFront = -128; 

  if(targetSpeedRevPercentLeftBack > 127) targetSpeedRevPercentLeftBack = 127;
  if(targetSpeedRevPercentRightBack > 127) targetSpeedRevPercentRightBack = 127;
  if(targetSpeedRevPercentRightBack < -128) targetSpeedRevPercentRightBack = -128;
  if(targetSpeedRevPercentLeftBack < -128) targetSpeedRevPercentLeftBack = -128;  

  int32_t targetPosTicksLeft =  targetPosRevLeft * MOTOR_GEARBOX_RATIO * MOTOR_TICKS_PER_MECH_REV;
  int32_t targetPosTicksRight =  targetPosRevRight * MOTOR_GEARBOX_RATIO * MOTOR_TICKS_PER_MECH_REV;

  targetPosTicksRight = -targetPosTicksRight;

  cout << "Rover theorical max speed (rev/min): " << roverMaxRevSpeed << endl;
  cout << "Target speed front left (rev/min): " << targetSpeedRevLeftFront << endl;
  cout << "Target speed back left (rev/min): " << targetSpeedRevLeftBack << endl;
  cout << "Target position left (rev): " << targetPosRevLeft << " (" << targetPosTicksLeft << " ticks)" << endl;
  cout << "Target speed right front (rev/min): " << targetSpeedRevRightFront << endl;
  cout << "Target speed right back (rev/min): " << targetSpeedRevRightBack << endl;
  cout << "Target position right (rev): " << targetPosRevRight << " (" << targetPosTicksRight << " ticks)" << endl;


  // Creating socket file descriptor 
  if((g_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ 
    perror("socket creation failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  // Configure network socket
  setsockopt(g_sockfd, IPPROTO_UDP, SO_BINDTODEVICE, devname.c_str(), devname.length());
  
  // Set the timeout for the reception socket
  tv.tv_sec = TIMEOUT_RX_SECOND;
  tv.tv_usec = 0;
  setsockopt(g_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  memset(&g_servaddr, 0, sizeof(g_servaddr)); 
  memset(&g_cliaddr, 0, sizeof(g_cliaddr)); 
  
  // Server information 
  g_servaddr.sin_family = AF_INET; // IPv4 
  g_servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  g_servaddr.sin_port = htons(LOCAL_PORT);

  // Client address information
  g_cliaddr.sin_family = AF_INET;
  g_cliaddr.sin_addr.s_addr = inet_addr(CLIENT_IP_ADDRESS);
  g_cliaddr.sin_port = htons(ROVER_PORT); 
  
  // Bind the socket with the server address 
  if(bind(g_sockfd, 
          (const struct sockaddr *)&g_servaddr, 
          sizeof(g_servaddr)) < 0 ){ 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  cout << "Send motor control commands..." << endl;
  setTargetSpeedRight(targetSpeedRevPercentRightFront, targetSpeedRevPercentRightBack);
  setTargetSpeedLeft(targetSpeedRevPercentLeftFront, targetSpeedRevPercentLeftBack);
  setTargetPositionRight(targetPosTicksRight);
  setTargetPositionLeft(targetPosTicksLeft);
  
  cout << "Start monitoring of motors..." << endl;

  struct timeval tic;
  struct timeval toc;  
  gettimeofday(&tic, NULL);
  long int ticms = tic.tv_sec * 1000 + tic.tv_usec / 1000;

  logFile << "Data format: <time in ms since beginning>,<position ticks FL>,<position ticks FR>,<position ticks RL>,<position ticks RR>,<current mA FL>,<current mA FR>,<current mA RL>,<current mA RR>" << endl;
  while(g_runApp){
    usleep(50000);
    getPositions(&positionFrontLeft, &positionFrontRight, &positionRearLeft, &positionRearRight);
    usleep(50000);
    getCurrents(&currentFrontLeft, &currentFrontRight, &currentRearLeft, &currentRearRight);

    gettimeofday(&toc, NULL);
    long int tocms = toc.tv_sec * 1000 + toc.tv_usec / 1000;  

    double currentRearRightDouble;
    double currentRearLeftDouble;
    double currentFrontRightDouble;
    double currentFrontLeftDouble;

    currentRearLeftDouble = fixedPointToDouble(currentRearLeft) * MAX_MEASURABLE_CURRENT;
    currentFrontLeftDouble = fixedPointToDouble(currentFrontLeft) * MAX_MEASURABLE_CURRENT;
    currentFrontRightDouble = fixedPointToDouble(currentFrontRight) * MAX_MEASURABLE_CURRENT;
    currentRearRightDouble = fixedPointToDouble(currentRearRight) * MAX_MEASURABLE_CURRENT;

    logFile << tocms - ticms << "," << positionFrontLeft << "," << positionFrontRight << "," << positionRearLeft << "," << positionRearRight << "," << currentFrontLeftDouble << "," << currentFrontRightDouble << "," << currentRearLeftDouble << "," << currentRearRightDouble << endl;
    cout << "Positions - FL: " << positionFrontLeft << " RL: " << positionRearLeft << " FR: " << positionFrontRight << " RR:" << positionRearRight << endl;
    cout << "Currents - FL:" << currentFrontLeftDouble << " RL: " << currentRearLeftDouble << " FR:" << currentFrontRightDouble << " RR:" << currentRearRightDouble << endl;
  }

  //stop();

  cout << "Exit application..." << endl;

  return 0; 
} 
