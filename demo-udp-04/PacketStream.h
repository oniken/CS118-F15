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
#include "Packet.h"
using namespace std;
using std::ios;

#define MAX_PACKET_SIZE 1024

using namespace std;

class PacketStream {
    public:
        PacketStream(int startingseq);
        PacketStream();//for complilation purposes
        ~PacketStream();
        int initFile(char* filename);
        Packet* get(int x);
        streampos getFileSize();
        int getNumOfPacks();
        int getSizeOfPacket(int i);
        int setDataSize(int s);
        int insert(Packet p, int pos);
    private:
        int start_seq;
        int max_seq;
        streampos size;
        Packet* data;
        int packetNumber;
        bool flg;
};
