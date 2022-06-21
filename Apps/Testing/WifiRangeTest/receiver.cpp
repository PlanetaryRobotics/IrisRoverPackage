/* simple.c
   Simple libftdi usage example
   This program is distributed under the GPL, version 2
*/
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include <ctime>

#include <stdlib.h>
#include <ftdi.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

#define MAX_SIZE_TEST_PACKET  2048

ofstream g_logFile;
int g_msgReceived;
float g_rssiTotal;
float g_snrTotal;
float g_rssiAverage;
float g_snrAverage;
struct ftdi_context *g_ftdi;
bool g_runApp;

void quitApplication(int s){
  g_runApp = false;
}

int main(void)
{
  int ret;
  int bytesRead;
  uint32_t bytesToRead;
  time_t now = time(0);
  struct ftdi_version_info version;
  struct sigaction sigIntHandler;
  string testDescription;

  sigIntHandler.sa_handler = quitApplication;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  g_rssiTotal = 0;
  g_snrTotal = 0;
  g_msgReceived = 0;
  g_runApp = true;

  // Create log file
  string nowDate = ctime(&now);
  g_logFile.open(nowDate.c_str());

  if ((g_ftdi = ftdi_new()) == 0){
    fprintf(stderr, "ftdi_new failed\n");
    return EXIT_FAILURE;
  }

  if ((ret = ftdi_usb_open(g_ftdi, 0x0403, 0x6001)) < 0){
    cout << "Fail to open device!" << endl;
    return EXIT_FAILURE;
  }

  ftdi_set_baudrate(g_ftdi, 115200);
  ftdi_set_line_property(g_ftdi, 
                        ftdi_bits_type::BITS_8,
                        ftdi_stopbits_type::STOP_BIT_1,
                        ftdi_parity_type::NONE);

  unsigned char * buf;

  buf = (unsigned char *) malloc(MAX_SIZE_TEST_PACKET * sizeof(unsigned char));
  
  cout << "Enter short description of test:" << endl;
  cin >> testDescription;
  g_logFile << testDescription.c_str();
  g_logFile << endl;

  while(g_runApp){
    bytesRead = ftdi_read_data(g_ftdi, buf, 10);
    if(bytesRead > 0){

      memcpy(&bytesToRead,
             buf+4,
             sizeof(bytesToRead));

      if(bytesToRead < MAX_SIZE_TEST_PACKET){
        bytesRead = ftdi_read_data(g_ftdi, buf+10, bytesToRead);
        if(bytesRead > 0){
          g_msgReceived++;
          g_rssiTotal += (float)(int8_t)(buf[8]);
          g_snrTotal += buf[9];
          printf("Message received #: %d, RSSI: %d , SNR: %d \n", 
                 g_msgReceived,
                 (char)buf[8],
                 buf[9]);
        }
      }
    }
  }

  cout << "--------------------------------------------" << endl;
  cout << "Done!" << endl;

  if(g_msgReceived > 0){
    g_rssiAverage = g_rssiTotal / (float)g_msgReceived;
    g_snrAverage = g_snrTotal / (float)g_msgReceived;
  } 

  g_logFile << "Summary:" << endl;
  g_logFile << "Message Received: " << g_msgReceived << endl;
  g_logFile << "Average RSSI: " << g_rssiAverage << endl;
  g_logFile << "Average SNR: " << g_snrAverage << endl;

  
  if ((ret = ftdi_usb_close(g_ftdi)) < 0){
      fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(g_ftdi));
      ftdi_free(g_ftdi);
      g_runApp = false;
  }

  ftdi_free(g_ftdi);


  return EXIT_SUCCESS;
}