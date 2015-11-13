#include <iostream>
#include <vector>
#include <queue>
#include <list>

#define MAX_PACKET_SIZE 1024

using namespace std;

class Packet {
    public:

    private:
        char* data;
        int seq;
        int ACK;
        bool isAck;
        bool isCorrupted;
};

class Window {
    public:

    private:
        int window_length;
        list<Packet> packets;
        // 
        struct comparator {
             bool operator()(int i, int j) {
                  return i > j;
                   }
        };
        // sequence number
        priority_queue<int, std::vector<int>, comparator> minHeap;
};
