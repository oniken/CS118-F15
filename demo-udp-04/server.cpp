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
#include <vector>
#include <list>
#include <ctime>
#include <set>
#include <cstdio>
#include <string>
#include <regex.h>
#include <sys/stat.h>
#include <errno.h>
#include <sstream>
#include <algorithm>
#include "port.h"
#include "PacketStream.h"

#define BUFSIZE 1035
#define WINDOW_SIZE 5

using namespace std;
using std::ios;

struct comparator {
    bool operator()(int i, int j) {
        return i > j;
    }
};

int main(int argc, char **argv)
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */
	int fd;				/* our socket */
	int msgcnt = 0;			/* count # of messages we received */
	char buf[BUFSIZE];	/* receive buffer */
    int portno = atoi(argv[1]);
    int cwnd = atoi(argv[2]);
    double loss = atof(argv[3]);
    double corruption = atof(argv[4]);
    if (loss >= 1 || corruption >= 1) {
        cout << "Probability of loss and corruption must be less than 1" << endl;
        exit(1);
    }


	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(portno);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}
	while(1) {
		bzero(buf, BUFSIZE);
		printf("waiting on port %d\n", portno);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
		}
		else {
			printf("uh oh - something went wrong!\n");
			continue;
		}
		Packet nPackets;
		PacketStream packetsToSend;
		int s=0;
		bool flg=false;
		if(packetsToSend.initFile(buf)==0) {
			stringstream convert;
			convert<<packetsToSend.getFileSize();
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
		struct timeval tv;
		tv.tv_sec=5;
		tv.tv_usec=0;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
		do {
			printf("sending response \"%s\"\n", nPackets.getData());
			if (sendto(fd, (char*)&nPackets, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
				perror("sendto");
			recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &addrlen);
			if (recvlen >= 0) {
		        Packet ack = (Packet)buf;
		        printf("received AckData 0: %s\n", ack.getData());
		        printf("received AckSeq 0: %s\n", ack.getSeq());
		        if(!ack.isCorrupted())
		        	break;
		    }
		    bzero(buf, BUFSIZE);
		}while(recvlen<0);
		if(flg) {
            list <int> sent_packets;
            set<int> acks;
            for (int i = 0; i < min(packetsToSend.getNumOfPacks(), WINDOW_SIZE); i++) {
                Packet curr = packetsToSend.get(i);
                printf("sending Packet num : %d\n", curr.getSeq());
                curr.setIsLost(loss);
                printf("sending Packet corrupted : %d\n", curr.isCorrupted());
		        if (sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			    perror("sendto");
                sent_packets.push_back(i);
            }

            while ((recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen))) {
                if (recvlen < 0) {
                    list<int>::iterator it = sent_packets.begin();
                    set<int>::iterator ack_it = acks.begin();
                    while (it != sent_packets.end()) {
                        if (ack_it == acks.end()) {
                            Packet curr = packetsToSend.get(*it);
                            sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen);
                            it++;
                        }
                        else {
                            if (*it == *ack_it) {
                                it++;
                                ack_it++;
                            }
                            else {
                                Packet curr = packetsToSend.get(*it);
                                sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen);
                                it++;
                            }
                        }
                    }    
                }
                else {
                    Packet num = (Packet)buf;
                    printf("Received ACKData %s\n", num.getData());
                    printf("Received ACKSeq %d\n", num.getSeq());
                    if (atoi(num.getData()) - 1 >= *(sent_packets.begin())) {
                        acks.insert(atoi(num.getData()) - 1);
                        cout << "Inserted an ACK into data structure" << endl;
                    }
                    list<int>::iterator it = sent_packets.begin();
                    set<int>::iterator ack_it = acks.begin();
                    while (ack_it != acks.end() && *ack_it == *it) {
                        printf("Entered here\n");
                        int ack_target = *ack_it;
                        int list_target = *it;
                        ack_it++;
                        it++;
                        printf("acks size before erase: %d\n", acks.size());
                        acks.erase(ack_target);
                        printf("acks size after erase: %d\n", acks.size());
                        sent_packets.pop_front();
                    }
                         printf("The size of list is %d\n", sent_packets.size());
                         printf("The back is %d\n", sent_packets.back());
                         printf("%d\n", packetsToSend.getNumOfPacks() - 1);
                    while (sent_packets.size() < WINDOW_SIZE && sent_packets.back() < packetsToSend.getNumOfPacks() - 1) {

                         printf("Sending packet %d since we received ACK %s\n", sent_packets.back() + 1, num.getData());
                         Packet curr = packetsToSend.get(sent_packets.back()+1);
	                     if (sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
		                 perror("sendto");
                         sent_packets.push_back(sent_packets.back() + 1);
                         cout << "Pushed into list " << sent_packets.back()<< endl;
                    }
                    if (sent_packets.empty()) {
                        printf("Finished file transfer\n");
                        break;
                    }
                }
                bzero(buf, BUFSIZE);
            }
		}
		bzero(buf, BUFSIZE);
	}
}
