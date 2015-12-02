#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <iostream>
#include <fstream>
#include <string.h>
#include <strings.h>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
using namespace std;
#define PAYLOAD_SIZE 1024
struct packet {
            char data[PAYLOAD_SIZE+1];
            char seq[9]; 
        //int seq;
        //int ACK;
        //bool Ack;
            bool Corrupted;
        };
// packet_stream(filename, startingseq, modmaxseq);
class Packet {
    public:
        Packet(char* stuff);
        Packet();
        //void setIsAck(bool x);
        //bool isAck();
        bool isCorrupted();
        void setIsCorrupted(float x);
        //void setSeqAckNum(int x, int y);
        //int getACK();
        int getSeq();
        void setData(char* c);
        char* getData();
        //void setAck(int x);
        void setSeq(int x);
    private:
        char data[PAYLOAD_SIZE+1];
        char seq[9]; 
        //int seq;
        //int ACK;
        //bool Ack;
        bool Corrupted;
        
};
