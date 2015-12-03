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
#define BUFLEN 1034
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
	while(1) {
		printf("Please enter file name: ");
	    bzero(buf, BUFLEN);
	    fgets(buf, BUFLEN,stdin);
	    string fileName=buf;
		/* now let's send the messages */
		struct timeval tv;
		tv.tv_sec=2;
		tv.tv_usec=0;
		Packet num;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
		do{
			printf("Sending file request packet for file %s to %s port %d\n", buf, server, SERVICE_PORT);
			if (sendto(fd, fileName.c_str(),fileName.size()-1, 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
				exit(1);
			}
			bzero(buf, BUFLEN);
			/* now receive an acknowledgement from the server */
			recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen);
			if (recvlen >= 0) {
		        num = (Packet)buf;
		        printf("received message: \"%s\"\n", num.getData());
		    }
		}while(recvlen<0||num.isCorrupted());

		if(strcmp(num.getData(),"-1")==0) {
			continue;
		}
		long fileSize=atol(num.getData());
	    double p=fileSize/MAX_PACKET_SIZE;
        int nPackets=ceil(p);
        long lastPacketSize=fileSize%(MAX_PACKET_SIZE);
        nPackets=(lastPacketSize)?nPackets+1:nPackets;
	    PacketStream packetstream;
	    packetstream.setDataSize(nPackets);
        bool *checked = new bool[nPackets];
        for (int i = 0; i < nPackets; i++) {
            checked[i] = false;
        }
        Packet ack0;
        ack0.setSeq(0);
        ack0.setData("0");
        printf("Sending Ack %d\n", ack0.getSeq());
		if (sendto(fd, (char*)&ack0, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		bzero(buf, BUFLEN);
        while ((recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen))) {
            if (recvlen > 0) {
                Packet curr = (Packet) buf;
                if (curr.getSeq() != 0) {
	            	if (sendto(fd, (char*)&ack0, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
	            		perror("sendto");
	            		exit(1);
	            	}
                }
                else {
                    packetstream.insert(curr, 0);
		        	bzero(buf, BUFLEN);
		        	stringstream convert;
		        	Packet toSend;
		        	toSend.setData("1");
		        	toSend.setSeq(1);
                    printf("Received packet %d\n", curr.getSeq());
                    printf("The ACKDATA is %s\n", toSend.getData());
                    printf("The ACKSEQ is %d\n", toSend.getSeq());
		        	if (sendto(fd, (char*)&toSend, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
		        		perror("sendto");
		        		exit(1);
		        	}
                    checked[0] = true;
		        	bzero(buf, BUFLEN);
                    break;
                }
            }
        }
        bool one=false;
        if(nPackets==1) {
            one=true;
        }
        bzero(buf, BUFLEN);
        if(!one) {
	        while ((recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen))) {
		        if (recvlen >= 0) {
		        	printf("entered recvlen\n");
		            Packet curr =(Packet) buf;
	             //   if (curr.isCorrupted()) {
	              //      printf("Detected corruption...\n");
	                    printf("Received seq number %d\n", curr.getSeq());
	               //     continue;
	              //  }
		            if(packetstream.insert(curr, curr.getSeq())==-1) {
		            	printf("Insertion in packet stream at larger than size.\n");
		            	continue;
		            }
	                else {
			        	bzero(buf, BUFLEN);
			        	printf("Sending packet %d to %s port %d\n", curr.getSeq() + 1, server, SERVICE_PORT);
			        	stringstream convert;
	                    convert << curr.getSeq() + 1;
	                    string c = convert.str();
	                    char lol[c.length()];
	                    for (int i = 0; i < c.size(); i++) {
	                        lol[i] = c[i];
	                    }
	                    lol[c.length()] = 0;
			        	Packet toSend;
	                    toSend.setData(lol);
			        	toSend.setSeq(curr.getSeq() + 1);
	                    if (curr.getSeq() + 1 <= nPackets) {
	                    printf("The ACKDATA is %s\n", toSend.getData());
	                    printf("The ACKSEQ is %d\n", toSend.getSeq());
			        	if (sendto(fd, (char*)&toSend, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			        		perror("sendto");
			        		exit(1);
			        	}
	                    cout << "Finished sending " << toSend.getSeq() << endl;
	                    checked[curr.getSeq()] = true;
	                    }
	                    bool file_done = false;
	                    for (int i = 0; i < nPackets; i++) {
	                        if (checked[i] == false) {
	                            break;
	                        }
	                        if (i == nPackets - 1) {
	                            file_done = true;
	                        }
	                    }
	                    if (file_done) break;
			        	bzero(buf, BUFLEN);
	                }
	            }
	            bzero(buf, BUFLEN);
	        }
		}
        /*
		for (i=0; i < nPackets; i++) {
			bzero(buf, BUFLEN);
			printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
			stringstream convert;
			Packet toSend;
			toSend.setAck(i);
			if (sendto(fd, (char*)&toSend, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
				exit(1);
			}
			bzero(buf, BUFLEN);
			recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
	        if (recvlen >= 0) {
	        	buf[recvlen] = 0;
	            printf("received message: \"%s\"\n", buf);
	        }
	        num=(Packet) buf;
	        if(packetstream.insert(num, i)==-1) {
	        	printf("Insertion in packet stream at larger than size.\n");
	        	break;
	        }
		}

*/
		unsigned char* op2 = (unsigned char*) malloc(fileSize);
		for(int i=0;i<nPackets;i++) {
            if (i == nPackets - 1) {
			memcpy(&op2[i*MAX_PACKET_SIZE], &(packetstream.get(i).getData()[0]), lastPacketSize);
            }
            else {
			memcpy(&op2[i*MAX_PACKET_SIZE], &(packetstream.get(i).getData()[0]), MAX_PACKET_SIZE);
            }
		}
	    if (nPackets > 0) {
			fileName.erase(remove(fileName.begin(),fileName.end(),'\n'), fileName.end());
			FILE* f = fopen(fileName.c_str(), "w+");
			if(f==NULL)
				printf("Failed to open write file\n");
			//fwrite(op.c_str(),sizeof(char), op.length(),f);
			fwrite(op2,sizeof(char), fileSize,f);
		    printf("Received file %s", fileName.c_str());
		    fclose(f);
	    }
        free(op2);
		close(fd);
	}
	return 0;
}
