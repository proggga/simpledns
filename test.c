#include<stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFLEN 200
void die(char *s)
{
    perror(s);
    exit(1);
}


int main(){
       int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
       int status, errno = 0;
       unsigned int sec  = 10 * pow(10, 6);
       struct sockaddr_in addr, si_other;
       memset((char *) &si_other, 0, sizeof(si_other));
       si_other.sin_family = AF_INET;
       si_other.sin_port = htons(53);
       si_other.sin_addr.s_addr = inet_addr("8.8.8.8");
       memset(&addr, 0, sizeof(struct sockaddr_in));
       addr.sin_family = AF_INET;
       addr.sin_port = 0;
       addr.sin_addr.s_addr = INADDR_ANY; 
       if ( (status = bind(udp_socket, (struct sockaddr *) &addr, sizeof(addr))) < 0)
               printf("bind error with port %s\n", strerror(errno));
       
       char buf[BUFLEN] = "1313123123123\0";
       int recv_len = strlen(buf);
       int slen = sizeof(si_other);
       if (sendto(udp_socket, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
       {
            die("sendto()");
       }
       usleep(sec);
       close(udp_socket);
}
