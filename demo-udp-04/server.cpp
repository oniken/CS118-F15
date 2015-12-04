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
#include <math.h>
#include "port.h"
#include "PacketStream.h"

#define BUFSIZE 1036
#define WINDOW_SIZE 5
#define TIMEOUT_SEC 5

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
    int cwnd = (int)floor(atoi(argv[2])/sizeof(Packet));
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
            Packet fileName = (Packet) buf;
            if (fileName.getSeq() == -2) {
                Packet lastfyn;
                lastfyn.setSeq(-2);
	            lastfyn.setIsLost(loss);
	            lastfyn.setIsCorrupted(corruption);
				if (sendto(fd, (char*)&lastfyn, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
	                exit(1);
	            continue;
            }
            if (fileName.getSeq() != -1) {
                cout << "Didn't receive a filename" << endl;
                continue;
            }
            if(fileName.isLost()) {
            	cout << "Assuming packet is lost\n\n\n";
                bzero(buf, BUFSIZE);
                continue;
            }
            if(fileName.isCorrupted()) {
            	cout << "Assuming packet is corrupted\n\n\n";
                bzero(buf, BUFSIZE);
                continue;
            }
			printf("received message: \"%s\" \n", fileName.getData());
		}
		else {
			printf("uh oh - something went wrong!\n");
			continue;
		}
		Packet nPackets;
        nPackets.setSeq(-1);
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
			nPackets.setSeq(-1);
		}
		bzero(buf, BUFSIZE);
		struct timeval tv;
		tv.tv_sec=5;
		tv.tv_usec=0;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(struct timeval));
		do {
			printf("sending response \"%s\"\n", nPackets.getData());
            nPackets.setIsLost(loss);
            nPackets.setIsCorrupted(corruption);
			if (sendto(fd, (char*)&nPackets, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
				perror("sendto");
			recvlen = recvfrom(fd, buf, sizeof(Packet), 0, (struct sockaddr *)&remaddr, &addrlen);
			if (recvlen >= 0) {
		        Packet ack = (Packet)buf;
		        if(ack.isLost()) {
	            	cout << "Assuming packet is lost\n\n\n";
	                bzero(buf, BUFSIZE);
	                continue;
	            }
	            if(ack.isCorrupted()) {
	            	cout << "Assuming packet is corrupted\n\n\n";
	                bzero(buf, BUFSIZE);
	                continue;
	            }
                if (ack.getSeq() == -1) {
                    cout << "Received a filename again" << endl;
                    continue;
                }
		        if(!ack.isCorrupted())
		        	break;
                printf("received AckSeq 0: %d\n", ack.getSeq());
		    }
		    bzero(buf, BUFSIZE);
		}while(recvlen<0);
		if(flg) {
            list <pair<int, time_t> > sent_packets;
            set<int> acks;
            for (int i = 0; i < min(packetsToSend.getNumOfPacks(), cwnd); i++) {
                Packet curr = packetsToSend.get(i);
                printf("sending Packet num : %d\n", curr.getSeq());
                curr.setIsLost(loss);
            	curr.setIsCorrupted(corruption);
		        if (sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			    perror("sendto");
                sent_packets.push_back(make_pair(i, time(nullptr)));
            }
            bzero(buf, BUFSIZE);
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(struct timeval));
            while ((recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen))) {
                if (recvlen < 0) {
                    cout<<"Recvfrom timed out"<<endl;
                    list<pair<int, time_t> >::iterator it = sent_packets.begin();
                    set<int>::iterator ack_it = acks.begin();
                    while (it != sent_packets.end()) {
                        cout<<"Entered timed out while"<<endl;
                        if (ack_it == acks.end()) {
                            cout<<"Entered timed out outer if"<<endl;
                            Packet curr = packetsToSend.get(it->first);
            				curr.setIsCorrupted(corruption);
                            curr.setIsLost(loss);
                            int timeval=time(nullptr);
                            cout<<"timeval: "<<timeval<<endl;
                            double duration = (timeval - it->second);
                            if (duration >= TIMEOUT_SEC) {
                                cout<<"Packet timed out"<<endl;
                                sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen);
                                it->second = time(nullptr);
                            }
                            it++;
                        }
                        else {
                            cout<<"Entered outer else"<<endl;
                            if (it->first == *ack_it) {
                                cout<<"Entered inner if"<<endl;
                                it++;
                                ack_it++;
                            }
                            else {
                                cout<<"Entered inner else"<<endl;
                                Packet curr = packetsToSend.get(it->first);
                                curr.setIsLost(loss);
                                curr.setIsCorrupted(corruption);
                                int timeval=time(nullptr);
                                cout<<"timeval: "<<timeval<<endl;
                                double duration = (timeval - it->second);
                                if (duration >= TIMEOUT_SEC) {
                                    cout<<"Packet timed out"<<endl;
                                    sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen);
                                    it->second = time(nullptr);
                                }
                                it++;
                            }
                        }
                    }    
                }
                else {
                    Packet num = (Packet)buf;
                    if(num.isLost()) {
		            	cout << "Assuming packet is lost\n";
		            	cout<<"Lost Packet Contents:\n";
		            	cout<<"Seq: "<<num.getSeq()<<endl;
		                bzero(buf, BUFSIZE);
		                continue;
		            }
		            if(num.isCorrupted()) {
		            	//TODO: FIX!
		            	cout << "Assuming packet is corrupted\n\n\n";
		                bzero(buf, BUFSIZE);
                        if (!sent_packets.empty()) {
    		                Packet p=packetsToSend.get(sent_packets.front().first);
    		                p.setIsLost(loss);
                            p.setIsCorrupted(corruption);
                            sendto(fd, (char*)&p, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen);
                        }
		                continue;
		            }
                    if (num.getSeq() == -2) {
                        cout<<"Fin received from client."<<endl;
                        Packet curr;
                        curr.setSeq(-2);
                        curr.setIsLost(loss);
                        curr.setIsCorrupted(corruption);
	                    sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0;
                        printf("Finished file transfer\n");
                        break;
                    }
                    printf("Received ACKData %s\n", num.getData());
                    printf("Received ACKSeq %d\n", num.getSeq());
                    if (num.getSeq() - 1 >= (sent_packets.begin()->first)) {
                        acks.insert(num.getSeq() - 1);
                        cout << "Inserted an ACK into data structure" << endl;
                    }
                    list<pair<int, time_t> >::iterator it = sent_packets.begin();
                    set<int>::iterator ack_it = acks.begin();
                    while (ack_it != acks.end() && *ack_it == it->first) {
                        printf("Entered here\n");
                        int ack_target = *ack_it;
                        int list_target = it->first;
                        ack_it++;
                        it++;
                        printf("acks size before erase: %d\n", acks.size());
                        acks.erase(ack_target);
                        printf("acks size after erase: %d\n", acks.size());
                        sent_packets.pop_front();
                    }
                     printf("The size of list is %d\n", sent_packets.size());
                     printf("The back is %d\n", sent_packets.back().first);
                     printf("%d\n", packetsToSend.getNumOfPacks() - 1);
                    while (sent_packets.size() < cwnd && sent_packets.back().first < packetsToSend.getNumOfPacks() - 1) {

                         printf("Sending packet %d since we received ACK %d\n", sent_packets.back().first + 1, num.getSeq());
                         Packet curr = packetsToSend.get(sent_packets.back().first+1);
                         curr.setIsLost(loss);
                         curr.setIsCorrupted(corruption);
	                     if (sendto(fd, (char*)&curr, sizeof(Packet), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
		                 perror("sendto");
                         sent_packets.push_back(make_pair(sent_packets.back().first + 1, time(nullptr)));
                         cout << "Pushed into list " << sent_packets.back().first<< endl;
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
