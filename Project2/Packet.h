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
// packet_stream(filename, startingseq, modmaxseq);
class Packet {
    public:
        ~Packet();
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
    private:
        char* data;
        int seq;
        int ACK;
        bool Ack;
        bool Corrupted;
};
