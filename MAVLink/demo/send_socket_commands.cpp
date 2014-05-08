/** This example is public domain. */


/**
 * @file
 *   @brief The serial interface process
 *
 *   This process connects any external MAVLink UART device and prints data
 *
 *   @author Lorenz Meier, <lm@inf.ethz.ch>
 *
 */

#include <common/mavlink.h>

// Standard includes
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <fstream>
// Serial includes
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#ifdef __linux
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// Latency Benchmarking
#include <sys/time.h>
#include <time.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#ifndef INT16_MAX
#define INT16_MAX 0x7fff
#endif
#ifndef INT16_MIN
#define INT16_MIN (-INT16_MAX - 1)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX 0xffff
#endif

using std::string;
using namespace std;

struct timeval tv;                ///< System time

// Settings

int sysid = 1;             ///< The unique system id of this MAV, 0-127. Has to be consistent across the system
int compid = 50;
int serial_compid = 0;
bool silent = false;              ///< Wether console output should be enabled
bool verbose = false;             ///< Enable verbose output
bool debug = false;               ///< Enable debug functions and output
int sockfd;

void quit_handler(int sig);

void quit_handler(int sig) {
        printf("Exiting on user request.\n");
        exit(0);
}

/**
 * @brief Serial function
 *
 * This function sends data out
 * and forwards the data once received.
 */
int serial_send(int serial_fd)
{
        int fd = serial_fd;
        
        mavlink_status_t lastStatus;
        lastStatus.packet_rx_drop_count = 0;

        unsigned loopcounter = 0;

        char buf[300];
        
        /*
         * CH 1: Roll Left = low PWM – Roll Right = High PWM
         * CH 2: Pitch Forward = low PWM – Pitch Back = High PWM
         * CH 3: Low Throttle = low PWM – High Throttle = High PWM
         * CH 4: Yaw Left = low PWM – Yaw Right = High PWM 
         */
        
        
        /* send one control packet every ~20 ms (~50 Hz) */
        
        while (1)
        {
                
          
                mavlink_message_t heatbeat_message;
                mavlink_heartbeat_t heatbeat;
                
                heatbeat.type = MAV_TYPE_GCS;
                heatbeat.autopilot = MAV_AUTOPILOT_INVALID;
                heatbeat.system_status = MAV_STATE_ACTIVE;
                
                
                mavlink_msg_heartbeat_encode(1, 50, &heatbeat_message, &heatbeat);
                unsigned len2 = mavlink_msg_to_send_buffer((uint8_t*)buf, &heatbeat_message);
                int n = write(fd, buf, len2);
                if (n < 0) 
                  printf("ERROR writing to socket");
                tcdrain(fd);


                /*
                
                mavlink_set_quad_swarm_roll_pitch_yaw_thrust_t sp;

                sp.group = 0;

                /* set rate mode, set zero rates and 20% throttle *
                sp.mode = MAVLINK_OFFBOARD_CONTROL_MODE_ATTITUDE;

                sp.roll[0] = INT16_MAX * 0.0f;
                sp.pitch[0] = INT16_MAX * 0.0f;
                sp.yaw[0] = INT16_MAX * 0.0f;
                sp.thrust[0] = UINT16_MAX * 0.2f;


                /* send from system 200 and component 0 *
                mavlink_msg_set_quad_swarm_roll_pitch_yaw_thrust_encode(200, 0, &message, &sp);*/
                
                mavlink_message_t message;

                mavlink_rc_channels_override_t rccontrol;
                
                rccontrol.chan1_raw = 1500; //roll
                rccontrol.chan2_raw = 1900; //pitch
                rccontrol.chan3_raw = 1500; //throttle
                rccontrol.chan4_raw = 1500; //yaw
                
                //rccontrol.target_system = 1;
                //rccontrol.target_component = 1;
                
                mavlink_msg_rc_channels_override_encode(1, 50, &message, &rccontrol);
                
                unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &message);
                printf("before write\n");
                /* write packet via serial link */
                printf("write: %d \n", write(fd, buf, len));
                /* wait until all data has been written */
                tcdrain(fd);
                printf("after write\n");

                if (loopcounter % 100 == 0) {
                        printf("Sent %d packets\n", loopcounter);
                }

                /* not exactly 20 ms, but close */
                usleep(20000);
                loopcounter++;
        }
        return 0;
}

//http://www.linuxhowtos.org/C_C++/socket.htm
int main(int argc, char **argv) {

    /* setup CTRL-C handling */
    signal(SIGINT,quit_handler);
  
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

    // Run indefinitely while the serial loop handles data
    if (!silent) printf("\nREADY, sending data.\n");

    // while(true) wait loop
    serial_send(sockfd);

    close(sockfd);


    return 0;
}
