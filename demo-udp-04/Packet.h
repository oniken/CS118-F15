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
using namespace std;
#define PAYLOAD_SIZE 1024
// packet_stream(filename, startingseq, modmaxseq);
class Packet {
    public:
        Packet(char* stuff);
        Packet();
        void setIsAck(bool x);
        bool isAck();
        bool isCorrupted();
        void setIsCorrupted(float x);
        void setSeqAckNum(int x, int y);
        int getACK();
        int getSeq();
        void setData(char* c);
        char* getData();
        void setAck(int x);
        void setSeq(int x);
    private:
        char data[PAYLOAD_SIZE+1];
        int seq;
        int ACK;
        bool Ack;
        bool Corrupted;
};
