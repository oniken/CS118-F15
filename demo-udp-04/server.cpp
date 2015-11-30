#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <strings.h>
#include <sys/wait.h> /* for the waitpid() system call */
#include <signal.h> /* signal name macros, and the kill() prototype */
#include <iostream>
#include <fstream>
#include <string>
#include <regex.h>
#include <sys/stat.h>
#include <errno.h>
#include <sstream>
#include "port.h"
#include "PacketStream.h"
#define BUFSIZE 1040
using namespace std;
using std::ios;
int main(int argc, char **argv)
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */
	int fd;				/* our socket */
	int msgcnt = 0;			/* count # of messages we received */
	char buf[BUFSIZE];	/* receive buffer */


	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	while(1) {
		bzero(buf, BUFSIZE);
		printf("waiting on port %d\n", SERVICE_PORT);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
		}
		else
			printf("uh oh - something went wrong!\n");
		Packet nPackets;
		PacketStream packetsToSend;
		int s=0;
		bool flg=false;
		if(packetsToSend.initFile(buf)==0) {
			stringstream convert;
			convert<<packetsToSend.getNumOfPacks();
			string c=convert.str();
			char lol[c.length()];
			for(int i=0;i<c.length();i++)
				lol[i]=c[i];
			lol[c.length()]=0;
			nPackets.setData(lol);
			flg=true;
		}
		else
		{
			char* image="-1";
            s = 2;
			nPackets.setData(image);
		}
		bzero(buf, BUFSIZE);
		//sprintf(buf, "ack %d", msgcnt++);
		printf("sending response \"%s\"\n", nPackets.getData());
		if (sendto(fd, (char*)&nPackets, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			perror("sendto");
		if(flg) {
			/* now loop, receiving data and printing what we received */
			for (int i=0;i<packetsToSend.getNumOfPacks();i++) {
				bzero(buf, BUFSIZE);
				printf("waiting on port %d\n", SERVICE_PORT);
				recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
				if (recvlen > 0) {
					buf[recvlen] = 0;
					printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
				}
				else
					printf("uh oh - something went wrong!\n");
				int getNum=atoi(buf);
				sprintf(buf, "ack %d", msgcnt++);
				printf("sending response \"%s\"\n", buf);
				nPackets=*(packetsToSend.get(getNum));
				if (sendto(fd, (char*)&nPackets, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
					perror("sendto");
			}
            printf("Finished sending file!");
		}
		bzero(buf, BUFSIZE);
	}
	/* never exits */
}