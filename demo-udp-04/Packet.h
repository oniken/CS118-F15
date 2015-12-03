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
            char seq[9]; ;
            bool Corrupted;
        };
class Packet {
    public:
        Packet(char* stuff);
        Packet();
        ~Packet();
        bool isCorrupted();
        void setIsCorrupted(float x);
        int getSeq();
        void setData(char* c);
        char* getData();
        void setSeq(int x);
    private:
        char data[PAYLOAD_SIZE+1];
        char seq[9]; 
        bool Corrupted;
        
};
