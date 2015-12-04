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
#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sstream>
#include "port.h"
#include "PacketStream.h"
#define BUFLEN 1036
using namespace std;
using std::ios;
int main(int argc, char **argv)
{
	struct sockaddr_in myaddr, remaddr;
	int fd, i;
    socklen_t slen=sizeof(remaddr);
	char buf[BUFLEN];	/* message buffer */
	int recvlen;		/* # bytes in acknowledgement message */
	char *server = argv[1];	/* change this to use a different server */
    int portno = atoi(argv[2]);
    double loss = atof(argv[4]);
    double corrupted = atof(argv[5]);
    if (loss >= 1 || corrupted >= 1) {
        cout << "Probability of loss and corruption must be less than 1" << endl;
        exit(1);
    }

	/* create a socket */

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("Error creating socket\n");

	/* bind it to all local addresses and pick any port number */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("Bind failed");
		return 0;
	}       

	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(portno);
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	strcpy(buf, argv[3]);
    char fn[BUFLEN];
    strcpy(fn, buf);
	/* now let's send the messages */
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=50000;
	Packet num;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
	do{
		bzero(buf, BUFLEN);
		strcpy(buf, argv[3]);
		printf("Sending file request packet for file %s to %s port %d\n", buf, server, portno);
        num.setData(buf);
        num.setSeq(-1);
        num.setIsLost(loss);
        num.setIsCorrupted(corrupted);
		if (sendto(fd,(void*) &num,sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		bzero(buf, BUFLEN);
		/* now receive an acknowledgement from the server */
		recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen);
		if (recvlen >= 0) {
	        num = (Packet)buf;
	        if (num.isLost()) {
                cout << "Assuming packet is lost\n";
                bzero(buf, BUFLEN);
                continue;
            }
            if(num.isCorrupted()) {
            	cout << "Assuming packet is corrupted\n";
                bzero(buf, BUFLEN);
            	continue;
            }
            if(num.getSeq()!=-1) {
                bzero(buf, BUFLEN);
                continue;
            }
	        printf("Received message: \"%s\"\n", num.getData());
	    }
	}while(recvlen<0);
	tv.tv_sec=0;
	tv.tv_usec=0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
	if(strcmp(num.getData(),"-1")==0) {
        cout << "File " << fn << " did not exist" << endl;
		exit(1);
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
    ack0.setData("");
    ack0.setIsLost(loss);
    ack0.setIsCorrupted(corrupted);
    printf("Sending Ack %d\n", ack0.getSeq());
	if (sendto(fd, (void*)&ack0, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
		perror("sendto");
		exit(1);
	}
	bzero(buf, BUFLEN);
    while ((recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen))) {
        if (recvlen > 0) {
            Packet curr = (Packet) buf;
            if (curr.isLost()) {
                cout << "Assuming packet is lost\n";
                bzero(buf, BUFLEN);
                continue;
            }
            if(curr.isCorrupted()) {
            	cout << "Assuming packet is corrupted\n";
            	ack0.setIsLost(loss);
			    ack0.setIsCorrupted(corrupted);
			    printf("Retransmitting Ack %d\n", ack0.getSeq());
				if (sendto(fd, (void*)&ack0, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
					perror("sendto");
					exit(1);
				}
				bzero(buf, BUFLEN);
				continue;
            }
            if (curr.getSeq() == -1)  {
                bzero(buf, BUFLEN);
                continue;
            }
            if (curr.getSeq() != 0) {
            	ack0.setIsLost(loss);
            	ack0.setIsCorrupted(corrupted);
            	if (sendto(fd, (void*)&ack0, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
            		perror("sendto");
            		exit(1);
            	}
            }
            else {
                packetstream.insert(curr, 0);
	        	bzero(buf, BUFLEN);
	        	stringstream convert;
	        	Packet toSend;
	        	toSend.setData("");
	        	toSend.setSeq(1);
                printf("Received packet %d\n", curr.getSeq());
                printf("Sending Ack %d\n", toSend.getSeq());
                toSend.setIsLost(loss);
                toSend.setIsCorrupted(corrupted);
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
    	int smallestPacketNum=1;
        while ((recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen))) {
	        if (recvlen >= 0) {
	            Packet curr =(Packet) buf;
                if (curr.isLost()) {
                    cout << "Assuming packet is lost\n";
                    bzero(buf, BUFLEN);
                    continue;
                }
                if(curr.isCorrupted()) {
                	cout << "Assuming packet is corrupted\n";
                	Packet toSend;
                    toSend.setData("");
		        	toSend.setSeq(smallestPacketNum);
                    printf("Retransmitting Ack %d\n", toSend.getSeq());
                    toSend.setIsLost(loss);
                    toSend.setIsCorrupted(corrupted);
		        	if (sendto(fd, (void*)&toSend, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
		        		perror("sendto");
		        		exit(1);
		        	}
		        	bzero(buf, BUFLEN);
		        	continue;
                }
                printf("Received seq number %d\n", curr.getSeq());
	            if(packetstream.insert(curr, curr.getSeq())==-1) {
	            	continue;
	            }
                else {
		        	bzero(buf, BUFLEN);
		        	Packet toSend;
                    toSend.setData("");
		        	toSend.setSeq(curr.getSeq() + 1);
                    if (curr.getSeq() + 1 <= nPackets) {
                    printf("Sending Ack %d\n", toSend.getSeq());
                    toSend.setIsLost(loss);
                    toSend.setIsCorrupted(corrupted);
		        	if (sendto(fd, (void*)&toSend, sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
		        		perror("sendto");
		        		exit(1);
		        	}
                    checked[curr.getSeq()] = true;
                    }
                    bool file_done = false;
                    for (int i = 0; i < nPackets; i++) {
                        if (checked[i] == false) {
                        	smallestPacketNum=max(i,1);
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
		FILE* f = fopen(fn, "w+");
		if(f==NULL)
			printf("Failed to open write file\n");
		fwrite(op2,sizeof(char), fileSize,f);
	    printf("Received file %s\n", fn);
	    fclose(f);
    }
    tv.tv_sec=1;
	tv.tv_usec=0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
    while ((recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &slen))) {
    	if(recvlen<0) {
    		Packet curr_fyn;
            curr_fyn.setSeq(-2);
	        curr_fyn.setIsLost(loss);
	        curr_fyn.setIsCorrupted(corrupted);
            cout<<"Sending fin to server"<<endl;
			if (sendto(fd,(void*) &curr_fyn,sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
				exit(1);
			}
			continue;
    	}
        Packet fyn = (Packet) buf;
        if (fyn.getSeq() == -2) {
            cout<<"Received fin from server"<<endl;
            break;
        }
        else {
            Packet curr_fyn;
            curr_fyn.setSeq(-2);
	        curr_fyn.setIsLost(loss);
	        curr_fyn.setIsCorrupted(corrupted);
			if (sendto(fd,(void*) &curr_fyn,sizeof(Packet), 0, (struct sockaddr *)&remaddr, slen)==-1) {
				perror("sendto");
				exit(1);
			}
        }
    }
    free(op2);
	close(fd);
	return 0;
}
