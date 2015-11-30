/*
        demo-udp-03: udp-send: a simple udp client
	send udp messages
	This sends a sequence of messages (the # of messages is defined in MSGS)
	The messages are sent to a port defined in SERVICE_PORT 

        usage:  udp-send

        Paul Krzyzanowski
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // define structures like hostent
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <sys/wait.h> /* for the waitpid() system call */
#include <signal.h> /* signal name macros, and the kill() prototype */
#include <iostream>
#include <fstream>
#include <string>
#include <regex.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sstream>
#include "port.h"
#include "PacketStream.h"
#define BUFLEN 1024
using namespace std;
using std::ios;
int main(void)
{
	struct sockaddr_in myaddr, remaddr;
	int fd, i;
    socklen_t slen=sizeof(remaddr);
	char buf[BUFLEN];	/* message buffer */
	int recvlen;		/* # bytes in acknowledgement message */
	char *server = "127.0.0.1";	/* change this to use a different server */

	/* create a socket */

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	/* bind it to all local addresses and pick any port number */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}       

	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(SERVICE_PORT);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	/* get file */
	printf("Please enter file name: ");
    bzero(buf, BUFLEN);
    fgets(buf, BUFLEN-1,stdin);
    string fileName=buf;
	/* now let's send the messages */
	printf("Sending file request packet for file %s to %s port %d\n", buf, server, SERVICE_PORT);
	if (sendto(fd, buf, strlen(buf)-1, 0, (struct sockaddr *)&remaddr, slen)==-1) {
		perror("sendto");
		exit(1);
	}
	bzero(buf, BUFLEN);
	/* now receive an acknowledgement from the server */
    Packet num;
	recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
	if (recvlen >= 0) {
	    num=(Packet) buf;
        printf("received message: \"%s\"\n", num.getData());
    }
    int nPackets=atoi(num.getData());
    PacketStream packet_stream;
    packet_stream.setDataSize(nPackets);
	for (i=0; i < nPackets; i++) {
		bzero(buf, BUFLEN);
		printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
		stringstream convert;
		convert<<i;
		string c=convert.str();
		strcpy(buf, c.c_str());
		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		bzero(buf, BUFLEN);
		/* now receive an acknowledgement from the server */
		recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) {
        	buf[recvlen] = 0;	/* expect a printable string - terminate it */
            printf("received message: \"%s\"\n", buf);
        }
        num=(Packet) buf;
        if(packet_stream.insert(num, i)==-1) {
        	printf("Insertion in packet stream at larger than size.\n");
        	break;
        }
	}
	string op="";
	for(int i=0;i<nPackets;i++) {
		op+=(packet_stream.get(i))->getData();
	}
	ofstream ofs(fileName.c_str(), ofstream::out);

    ofs<<op;
    ofs.close();
	close(fd);
	return 0;
}
