#include <common/mavlink.h>
#include <ardupilotmega/ardupilotmega.h>

// Standard includes
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string.h>
#include <inttypes.h>
#include <fstream>
// Serial includes
#include <stdio.h>   /* Standard input/output definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#ifdef __linux
#include <sys/ioctl.h>
#endif

// Latency Benchmarking
#include <sys/time.h>
#include <time.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using std::string;
using namespace std;

struct timeval tv;                ///< System time

// Settings
int sysid = 1;                    ///< The unique system id of this MAV, 0-127. Has to be consistent across the system
int compid = 1;
int serial_compid = 0;
bool silent = false;              ///< Whether console output should be enabled
bool verbose = true;             ///< Enable verbose output
bool debug = true;               ///< Enable debug functions and output
int sockfd;



/**
 * @brief Serial function
 *
 * This function blocks waiting for serial data in it's own thread
 * and forwards the data once received.
 */
int serial_wait(int serial_fd)
{
        int fd = serial_fd;
        
        mavlink_status_t lastStatus;
        lastStatus.packet_rx_drop_count = 0;
        
        // Blocking wait for new data
        while (1)
        {
                //if (debug) printf("Checking for new data on serial port\n");
                // Block until data is available, read only one byte to be able to continue immediately
                //char buf[MAVLINK_MAX_PACKET_LEN];
                uint8_t cp;
                mavlink_message_t message;
                mavlink_status_t status;
                uint8_t msgReceived = false;

                if (read(fd, &cp, 1) > 0)
                {
                        // Check if a message could be decoded, return the message in case yes
                        msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);
                        if (lastStatus.packet_rx_drop_count != status.packet_rx_drop_count)
                        {
                                if (verbose || debug) printf("ERROR: DROPPED %d PACKETS\n", status.packet_rx_drop_count);
                                if (debug)
                                {
                                        unsigned char v=cp;
                                        fprintf(stderr,"%02x ", v);
                                }
                        }
                        lastStatus = status;
                }
                else
                {
                        if (!silent) fprintf(stderr, "ERROR: Could not read from fd %d\n", fd);
                }
                
                // If a message could be decoded, handle it
                if(msgReceived)
                {
                        //if (verbose || debug) std::cout << std::dec << "Received and forwarded serial port message with id " << static_cast<unsigned int>(message.msgid) << " from system " << static_cast<int>(message.sysid) << std::endl;
                        
                        // Do not send images over serial port
                        
                        // DEBUG output
                        if (debug)
                        {
                                fprintf(stderr,"Received serial data: ");
                                unsigned int i;
                                uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
                                unsigned int messageLength = mavlink_msg_to_send_buffer(buffer, &message);
                                if (messageLength > MAVLINK_MAX_PACKET_LEN)
                                {
                                        fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE\n");
                                }
                                else
                                {
                                        for (i=0; i<messageLength; i++)
                                        {
                                                unsigned char v=buffer[i];
                                                fprintf(stderr,"%02x ", v);
                                        }
                                        fprintf(stderr,"\n");
                                }
                        }
                        
                        //if (verbose || debug)
                                printf("Received message from serial with ID #%d (sys:%d|comp:%d):\n", message.msgid, message.sysid, message.compid);
                        
                        /* decode and print */


                        // For full MAVLink message documentation, look at:
                        // https://pixhawk.ethz.ch/mavlink/

                        // Only print every n-th message
                        static unsigned int scaled_imu_receive_counter = 0;

                        switch (message.msgid)
                        {
                                case MAVLINK_MSG_ID_HEARTBEAT:
                                
                                  printf("Got heartbeat message \n");
                                  
                                  mavlink_heartbeat_t heartbeat;
                                  
                                  mavlink_msg_heartbeat_decode(&message, &heartbeat);
                                  printf("\t type: %d \n", heartbeat.type);
                                  printf("\t autopilot: %d \n", heartbeat.autopilot);
                                  printf("\t base_mode: %d \n", heartbeat.base_mode);
                                  printf("\t system_status: %d \n", heartbeat.system_status);
                                  printf("\t mavlink_version: %d \n", heartbeat.mavlink_version);
                                  
                                  break;

                              case MAVLINK_MSG_ID_RC_CHANNELS_RAW: //doesn't work??
                                  printf("Got rc channels");
                                  
                                  mavlink_rc_channels_raw_t channels;
                                  
                                  mavlink_msg_rc_channels_raw_decode(&message, &channels);
                                  printf("\t rc1: %d \n", channels.chan1_raw);
                                  printf("\t rc2: %d \n", channels.chan2_raw);
                                  printf("\t rc3: %d \n", channels.chan3_raw);
                                  
                                  break;
                              
                              case MAVLINK_MSG_ID_HIGHRES_IMU: //doesn't work??
                                  if (scaled_imu_receive_counter % 50 == 0) {
                                      mavlink_highres_imu_t imu;
                                      mavlink_msg_highres_imu_decode(&message, &imu);

                                      printf("Got message HIGHRES_IMU (spec: https://pixhawk.ethz.ch/mavlink/#HIGHRES_IMU)\n");
                                      printf("\t time: %llu\n", imu.time_usec);
                                      printf("\t acc  (NED):\t% f\t% f\t% f (m/s^2)\n", imu.xacc, imu.yacc, imu.zacc);
                                      printf("\t gyro (NED):\t% f\t% f\t% f (rad/s)\n", imu.xgyro, imu.ygyro, imu.zgyro);
                                      printf("\t mag  (NED):\t% f\t% f\t% f (Ga)\n", imu.xmag, imu.ymag, imu.zmag);
                                      printf("\t baro: \t %f (mBar)\n", imu.abs_pressure);
                                      printf("\t altitude: \t %f (m)\n", imu.pressure_alt);
                                      printf("\t temperature: \t %f C\n", imu.temperature);
                                      printf("\n");
                                  }
                                  scaled_imu_receive_counter++;

                                  break;
                        }
                }
        }
        return 0;
}


//http://www.linuxhowtos.org/C_C++/socket.htm

int main(int argc, char **argv) {
  
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting");
    }
    //printf("Please enter the message: ");
    //bzero(buffer,256);
    //fgets(buffer,255,stdin);
    
    /*
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    */
    serial_wait(sockfd);
    
    close(sockfd);
    return 0;
}