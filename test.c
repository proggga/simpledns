#include<stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <math.h>

int main(){
       int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
       unsigned int sec  = 10 * pow(10, 6);
       usleep(sec);
       close(udp_socket);
}
