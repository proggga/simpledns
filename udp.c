/*
    Simple udp server
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
 

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
enum e_query_response {QUERY, RESPONSE};
enum e_opcode {QUERY_CODE, IQUERY, STATUS, EMPTY, NOTIFY, UPDATE};

// http://www-inf.int-evry.fr/~hennequi/CoursDNS/NOTES-COURS_eng/msg.html
//                                           1  1  1  1  1  1
//             0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |                      ID                       |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |QR|   Opcode  |AA|TC|RD|RA| Z|AD|CD|   RCODE   |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |                    QDCOUNT                    |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |                    ANCOUNT                    |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |                    NSCOUNT                    |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//           |                    ARCOUNT                    |
//           +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

struct header {
   unsigned short id;
   unsigned short raw_control;
   enum e_query_response qs;
   enum e_opcode opcode;
   unsigned char authoritatvie;
   unsigned char truncated;
   unsigned char recursion;
   unsigned char authenticated;
   unsigned char nochecking;
   unsigned char returncode;
};
void die(char *s)
{
    perror(s);
    exit(1);
}

void printBits(unsigned short num)
{
   for(int cnt=0, bit=(sizeof(unsigned short) * 8)-1; bit >= 0; bit--, cnt++)
   {
      if(cnt > 3){
	 printf(" ");
	 cnt = 0;
      }
      printf("%i", (num >> bit) & 0x01);
   }
   printf("\n");
}

unsigned short get_short(char *buffer, int position){
	unsigned short val = *((unsigned short*)(buffer + position));
	//val = (val>>8) | (val<<8); // convert le to be
	val = ntohs(val);
	return val;
}

 
int main(void)
{
    struct sockaddr_in si_me, si_other;
     
    int s, slen = sizeof(si_other), recv_len;
    socklen_t server_addr_len = slen;
    char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &server_addr_len)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

	struct header h;
	h.id = get_short(buf, 0);
	h.raw_control = get_short(buf, 2);
	printf("DNS Query id: %d\n", h.id);
	printf("Raw control bits: ");
	printBits(h.raw_control);
	
	// printf("Third: %04X id is %d\n", val1, val1);

        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
    return 0;
}
