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
using std::ios;

#define MAX_PACKET_SIZE 1024

using namespace std;

class Packet_Stream {
    public:
        Packet_Stream(int startingseq);
        Packet_Stream();//for complilation purposes
        ~Packet_Stream();
        int initFile(char filename[]);
        Packet* get(int x);
        streampos getFileSize();
        int getNumOfPacks();
        int getSizeOfPacket(int i);
    private:
        ifstream file;
        int start_seq;
        int max_seq;
        streampos size;
        Packet data[];
        int packetNumber;
};
